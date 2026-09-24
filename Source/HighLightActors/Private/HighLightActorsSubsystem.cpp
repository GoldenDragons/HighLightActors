// Copyright Epic Games, Inc. All Rights Reserved.

#include "HighLightActorsSubsystem.h"

#include "HighLightActorGather.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace HighLightActors::Private
{
	static constexpr int32 HighlightStencilValue = 211;
	static const TCHAR* OutlineMaterialObjectPath = TEXT("/HighLightActors/Materials/M_HighLightOutline.M_HighLightOutline");
}

void UHighLightActorsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EnsurePostProcessSetup();
}

void UHighLightActorsSubsystem::Deinitialize()
{
	ClearHighlights();
	TeardownPostProcess();
	Super::Deinitialize();
}

void UHighLightActorsSubsystem::EnsurePostProcessSetup()
{
	UWorld* World = GetWorld();
	if (!World || OutlinePostProcessVolume)
	{
		return;
	}

	UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, HighLightActors::Private::OutlineMaterialObjectPath);
	OutlineBaseMaterial = Base;
	if (!Base)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("HighLightActors: Missing outline material %s."),
			HighLightActors::Private::OutlineMaterialObjectPath);
		return;
	}

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, this);
	if (!MID)
	{
		UE_LOG(LogTemp, Error, TEXT("HighLightActors: 无法为轮廓材质创建 MID。"));
		return;
	}

	OutlineMID = MID;
	MID->SetVectorParameterValue(TEXT("OutlineColor"), HighlightColor);
	MID->SetScalarParameterValue(TEXT("OutlineWidth"), HighlightWidth);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags = RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APostProcessVolume* PPV = World->SpawnActor<APostProcessVolume>(APostProcessVolume::StaticClass(), FTransform::Identity, SpawnParameters);
	if (!PPV)
	{
		UE_LOG(LogTemp, Error, TEXT("HighLightActors: 无法生成后处理体积。"));
		OutlineMID = nullptr;
		return;
	}

	OutlinePostProcessVolume = PPV;
	PPV->bUnbound = true;
	PPV->Priority = 100000.f;
	PPV->BlendWeight = 0.f;
	PPV->AddOrUpdateBlendable(MID, 1.f);
}

void UHighLightActorsSubsystem::TeardownPostProcess()
{
	if (OutlinePostProcessVolume)
	{
		OutlinePostProcessVolume->Destroy();
		OutlinePostProcessVolume = nullptr;
	}
	OutlineMID = nullptr;
	OutlineBaseMaterial = nullptr;
}

void UHighLightActorsSubsystem::ApplyOutlineStyle(FLinearColor Color, float Width)
{
	HighlightColor = Color;
	HighlightWidth = FMath::Clamp(Width, 0.f, 16.f);
	if (OutlineMID)
	{
		OutlineMID->SetVectorParameterValue(TEXT("OutlineColor"), HighlightColor);
		OutlineMID->SetScalarParameterValue(TEXT("OutlineWidth"), HighlightWidth);
	}
}

void UHighLightActorsSubsystem::SetMaxPrimitivesToHighlight(int32 MaxCount)
{
	MaxPrimitivesToHighlight = FMath::Max(0, MaxCount);
}

void UHighLightActorsSubsystem::ClearHighlights()
{
	UE_LOG(LogTemp, Verbose, TEXT("HighLightActors: Clearing highlights."));
	for (FHighLightCapturedPrimitiveState& State : CapturedPrimitiveStates)
	{
		if (UPrimitiveComponent* Prim = State.Primitive.Get())
		{
			if (IsValid(Prim))
			{
				Prim->SetRenderCustomDepth(State.bHadRenderCustomDepth);
				Prim->SetCustomDepthStencilValue(State.PreviousStencilValue);
			}
		}
	}
	CapturedPrimitiveStates.Reset();
	if (OutlinePostProcessVolume)
	{
		OutlinePostProcessVolume->BlendWeight = 0.f;
	}
}

void UHighLightActorsSubsystem::ApplyHighlightToPrimitives(const TArray<UPrimitiveComponent*>& Primitives)
{
	TSet<UPrimitiveComponent*> UniquePrimitives;
	for (UPrimitiveComponent* Prim : Primitives)
	{
		if (IsValid(Prim))
		{
			UniquePrimitives.Add(Prim);
		}
	}

	int32 Applied = 0;
	const int32 Limit = MaxPrimitivesToHighlight;

	for (UPrimitiveComponent* Prim : UniquePrimitives)
	{
		if (!IsValid(Prim))
		{
			continue;
		}

		if (Limit > 0 && Applied >= Limit)
		{
			UE_LOG(LogTemp, Warning, TEXT("HighLightActors: 已达到 MaxPrimitivesToHighlight=%d，其余 Primitive 未高亮。"), Limit);
			break;
		}

		FHighLightCapturedPrimitiveState Cap;
		Cap.Primitive = Prim;
		Cap.bHadRenderCustomDepth = Prim->bRenderCustomDepth != 0;
		Cap.PreviousStencilValue = Prim->CustomDepthStencilValue;

		Prim->SetRenderCustomDepth(true);
		Prim->SetCustomDepthStencilValue(HighLightActors::Private::HighlightStencilValue);

		CapturedPrimitiveStates.Add(MoveTemp(Cap));
		++Applied;
	}

	if (Applied > 0 && OutlinePostProcessVolume)
	{
		OutlinePostProcessVolume->BlendWeight = 1.f;
	}
}

void UHighLightActorsSubsystem::InternalHighlightFromRoots(const TArray<AActor*>& Roots, bool bIncludeChildren,
	FLinearColor Color, float Width)
{
	ClearHighlights();
	EnsurePostProcessSetup();
	ApplyOutlineStyle(Color, Width);

	TArray<AActor*> DedupedActors;
	for (AActor* Root : Roots)
	{
		if (!IsValid(Root))
		{
			continue;
		}

		TArray<AActor*> BranchActors;
		HighLightActorGather::CollectActorHierarchy(Root, BranchActors, bIncludeChildren);
		for (AActor* A : BranchActors)
		{
			DedupedActors.AddUnique(A);
		}
	}

	TArray<UPrimitiveComponent*> Primitives;
	HighLightActorGather::CollectPrimitivesFromActors(DedupedActors, Primitives);
	ApplyHighlightToPrimitives(Primitives);
}

void UHighLightActorsSubsystem::HighlightActor(AActor* Root, bool bIncludeChildren, FLinearColor OutlineColor, float OutlineWidth)
{
	if (!IsValid(Root))
	{
		return;
	}
	InternalHighlightFromRoots({ Root }, bIncludeChildren, OutlineColor, OutlineWidth);
}

void UHighLightActorsSubsystem::HighlightActors(const TArray<AActor*>& Roots, bool bIncludeChildren,
	FLinearColor OutlineColor, float OutlineWidth)
{
	InternalHighlightFromRoots(Roots, bIncludeChildren, OutlineColor, OutlineWidth);
}

void UHighLightActorsSubsystem::HighlightActorsWithTag(FName Tag, bool bIncludeChildren,
	FLinearColor OutlineColor, float OutlineWidth)
{
	InternalHighlightFromRoots(FindActorsByTag(Tag), bIncludeChildren, OutlineColor, OutlineWidth);
}

void UHighLightActorsSubsystem::HighlightActorsWithTags(const TArray<FName>& Tags, bool bIncludeChildren,
	FLinearColor OutlineColor, float OutlineWidth)
{
	InternalHighlightFromRoots(FindActorsByTags(Tags), bIncludeChildren, OutlineColor, OutlineWidth);
}

TArray<AActor*> UHighLightActorsSubsystem::FindActorsByTag(FName Tag) const
{
	TArray<AActor*> Result;
	UWorld* World = GetWorld();
	if (!World || Tag.IsNone())
	{
		return Result;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (IsValid(*It) && It->ActorHasTag(Tag))
		{
			Result.Add(*It);
		}
	}
	return Result;
}

TArray<AActor*> UHighLightActorsSubsystem::FindActorsByTags(const TArray<FName>& Tags) const
{
	TArray<AActor*> Result;
	TSet<AActor*> Unique;

	for (FName Tag : Tags)
	{
		for (AActor* A : FindActorsByTag(Tag))
		{
			Unique.Add(A);
		}
	}

	Result.Reserve(Unique.Num());
	for (AActor* A : Unique)
	{
		Result.Add(A);
	}
	return Result;
}
