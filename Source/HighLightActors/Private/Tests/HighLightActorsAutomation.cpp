// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "HighLightActorsLibrary.h"
#include "HighLightActorsSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/AutomationTest.h"

namespace
{
UWorld* FindHighLightActorsAutomationWorld()
{
	if (!GEngine)
	{
		return nullptr;
	}

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* World = Context.World();
		if (IsValid(World) && (Context.WorldType == EWorldType::Editor ||
			Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game))
		{
			return World;
		}
	}
	return nullptr;
}

AActor* SpawnOutlinedTestActor(UWorld* World, UStaticMeshComponent*& OutMesh)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags = RF_Transient;
	AActor* Actor = World->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity, SpawnParameters);
	if (!Actor)
	{
		return nullptr;
	}

	OutMesh = NewObject<UStaticMeshComponent>(Actor);
	Actor->SetRootComponent(OutMesh);
	OutMesh->RegisterComponent();
	return Actor;
}

UMaterialInstanceDynamic* FindOutlineMID(UWorld* World)
{
	for (TActorIterator<APostProcessVolume> It(World); It; ++It)
	{
		if (It->HasAnyFlags(RF_Transient) && It->Priority == 100000.f)
		{
			for (const FWeightedBlendable& Blendable : It->Settings.WeightedBlendables.Array)
			{
				if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(Blendable.Object.Get()))
				{
					return MID;
				}
			}
		}
	}
	return nullptr;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FHighLightActorsOutlineStateTest,
	"Plugins.HighLightActors.OutlineRestoresStencilState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHighLightActorsOutlineStateTest::RunTest(const FString& Parameters)
{
	UWorld* World = FindHighLightActorsAutomationWorld();
	if (!TestNotNull(TEXT("World"), World))
	{
		return false;
	}

	UStaticMeshComponent* Mesh = nullptr;
	AActor* Actor = SpawnOutlinedTestActor(World, Mesh);
	if (!TestNotNull(TEXT("Actor"), Actor))
	{
		return false;
	}
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(17);

	UHighLightActorsSubsystem* Subsystem = World->GetSubsystem<UHighLightActorsSubsystem>();
	if (!TestNotNull(TEXT("Subsystem"), Subsystem))
	{
		Actor->Destroy();
		return false;
	}

	Subsystem->HighlightActor(Actor, false);
	TestTrue(TEXT("Highlight enables custom depth"), Mesh->bRenderCustomDepth != 0);
	TestEqual(TEXT("Highlight uses the outline stencil"), Mesh->CustomDepthStencilValue, 211);
	Subsystem->ClearHighlights();
	TestTrue(TEXT("Clear restores custom depth"), Mesh->bRenderCustomDepth != 0);
	TestEqual(TEXT("Clear restores original stencil"), Mesh->CustomDepthStencilValue, 17);

	Actor->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FHighLightActorsBlueprintLibraryTest,
	"Plugins.HighLightActors.BlueprintOutlineLibrary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FHighLightActorsBlueprintLibraryTest::RunTest(const FString& Parameters)
{
	UWorld* World = FindHighLightActorsAutomationWorld();
	if (!TestNotNull(TEXT("World"), World))
	{
		return false;
	}
	UHighLightActorsSubsystem* Subsystem = World->GetSubsystem<UHighLightActorsSubsystem>();
	if (!TestNotNull(TEXT("Subsystem"), Subsystem))
	{
		return false;
	}

	UMaterial* OutlineMaterial = LoadObject<UMaterial>(nullptr,
		TEXT("/HighLightActors/Materials/M_HighLightOutline.M_HighLightOutline"));
	if (!TestNotNull(TEXT("Outline material"), OutlineMaterial))
	{
		return false;
	}
	TestTrue(TEXT("Outline material uses the post process domain"), OutlineMaterial->MaterialDomain == MD_PostProcess);
	IConsoleVariable* CustomDepthJitter = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepthTemporalAAJitter"));
	if (TestNotNull(TEXT("Custom depth jitter setting"), CustomDepthJitter))
	{
		TestEqual(TEXT("Custom depth is stable after temporal AA"), CustomDepthJitter->GetInt(), 0);
	}

	for (const FName Name : { FName(TEXT("HighlightActor")), FName(TEXT("HighlightActors")),
		FName(TEXT("HighlightActorsWithTag")), FName(TEXT("HighlightActorsWithTags")),
		FName(TEXT("ClearHighlights")), FName(TEXT("SetMaxPrimitivesToHighlight")),
		FName(TEXT("GetMaxPrimitivesToHighlight")) })
	{
		const UFunction* Function = UHighLightActorsLibrary::StaticClass()->FindFunctionByName(Name);
		TestTrue(*FString::Printf(TEXT("%s is a Blueprint node"), *Name.ToString()),
			Function && Function->HasAnyFunctionFlags(FUNC_BlueprintCallable));
	}

	for (const FName Name : { FName(TEXT("HighlightActor")), FName(TEXT("HighlightActors")),
		FName(TEXT("HighlightActorsWithTag")), FName(TEXT("HighlightActorsWithTags")) })
	{
		const UFunction* Function = UHighLightActorsLibrary::StaticClass()->FindFunctionByName(Name);
		TestTrue(*FString::Printf(TEXT("%s has an outline color input"), *Name.ToString()),
			Function && Function->FindPropertyByName(TEXT("OutlineColor")));
		TestTrue(*FString::Printf(TEXT("%s has an outline width input"), *Name.ToString()),
			Function && Function->FindPropertyByName(TEXT("OutlineWidth")));
#if WITH_METADATA
		if (Function)
		{
			FLinearColor DefaultColor;
			TestTrue(*FString::Printf(TEXT("%s has a valid default color"), *Name.ToString()),
				DefaultColor.InitFromString(Function->GetMetaData(TEXT("CPP_Default_OutlineColor"))));
			TestEqual(*FString::Printf(TEXT("%s defaults to UE selection color"), *Name.ToString()),
				DefaultColor, FLinearColor(0.828f, 0.364f, 0.003f, 1.f));
			TestEqual(*FString::Printf(TEXT("%s defaults to one pixel"), *Name.ToString()),
				FCString::Atof(*Function->GetMetaData(TEXT("CPP_Default_OutlineWidth"))), 1.f);
		}
#endif
	}
	TestNull(TEXT("No separate Blueprint color setter"),
		UHighLightActorsLibrary::StaticClass()->FindFunctionByName(TEXT("SetHighlightColor")));
	TestNull(TEXT("No separate Blueprint color getter"),
		UHighLightActorsLibrary::StaticClass()->FindFunctionByName(TEXT("GetHighlightColor")));

	UStaticMeshComponent* FirstMesh = nullptr;
	UStaticMeshComponent* SecondMesh = nullptr;
	AActor* First = SpawnOutlinedTestActor(World, FirstMesh);
	AActor* Second = SpawnOutlinedTestActor(World, SecondMesh);
	if (!TestNotNull(TEXT("First actor"), First) || !TestNotNull(TEXT("Second actor"), Second))
	{
		if (First) First->Destroy();
		if (Second) Second->Destroy();
		return false;
	}

	First->Tags.Add(TEXT("OutlineTest"));
	UHighLightActorsLibrary::SetMaxPrimitivesToHighlight(World, 1);
	TestEqual(TEXT("Blueprint max setter"), UHighLightActorsLibrary::GetMaxPrimitivesToHighlight(World), 1);

	UHighLightActorsLibrary::HighlightActorsWithTag(World, TEXT("OutlineTest"), false);
	TestEqual(TEXT("Tagged actor is outlined"), FirstMesh->CustomDepthStencilValue, 211);
	TestFalse(TEXT("Untagged actor is untouched"), SecondMesh->bRenderCustomDepth != 0);
	UMaterialInstanceDynamic* MID = FindOutlineMID(World);
	if (TestNotNull(TEXT("Outline MID"), MID))
	{
		TestEqual(TEXT("Default matches UE selection color"), MID->K2_GetVectorParameterValue(TEXT("OutlineColor")),
			FLinearColor(0.828f, 0.364f, 0.003f, 1.f));
		TestEqual(TEXT("Default outline width is one pixel"), MID->K2_GetScalarParameterValue(TEXT("OutlineWidth")), 1.f);
	}
	UHighLightActorsLibrary::HighlightActor(World, Second, false, FLinearColor::Red, 3.f);
	TestFalse(TEXT("Previous selection is restored"), FirstMesh->bRenderCustomDepth != 0);
	TestEqual(TEXT("New actor is outlined"), SecondMesh->CustomDepthStencilValue, 211);
	if (MID)
	{
		TestEqual(TEXT("Call applies outline color"), MID->K2_GetVectorParameterValue(TEXT("OutlineColor")),
			FLinearColor::Red);
		TestEqual(TEXT("Call applies outline width"), MID->K2_GetScalarParameterValue(TEXT("OutlineWidth")), 3.f);
	}
	UHighLightActorsLibrary::ClearHighlights(World);
	TestFalse(TEXT("Clear restores new actor"), SecondMesh->bRenderCustomDepth != 0);

	UHighLightActorsLibrary::SetMaxPrimitivesToHighlight(World, 0);
	First->Destroy();
	Second->Destroy();
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
