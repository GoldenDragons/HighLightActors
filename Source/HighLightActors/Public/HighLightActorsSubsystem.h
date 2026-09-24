// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HighLightActorsSubsystem.generated.h"

class AActor;
class APostProcessVolume;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UPrimitiveComponent;

USTRUCT()
struct FHighLightCapturedPrimitiveState
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> Primitive = nullptr;

	UPROPERTY()
	bool bHadRenderCustomDepth = false;

	UPROPERTY()
	int32 PreviousStencilValue = 0;
};

/** Owns the outline post process and restores primitive state when selection changes. */
UCLASS(DisplayName = "High Light Actors Subsystem")
class HIGHLIGHTACTORS_API UHighLightActorsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight")
	void HighlightActor(AActor* Root, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight", meta = (AutoCreateRefTerm = "Roots"))
	void HighlightActors(const TArray<AActor*>& Roots, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight")
	void HighlightActorsWithTag(FName Tag, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight", meta = (AutoCreateRefTerm = "Tags"))
	void HighlightActorsWithTags(const TArray<FName>& Tags, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight")
	void ClearHighlights();

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight")
	void SetMaxPrimitivesToHighlight(int32 MaxCount);

	UFUNCTION(BlueprintPure, Category = "HighLightActors|Highlight")
	int32 GetMaxPrimitivesToHighlight() const { return MaxPrimitivesToHighlight; }

private:
	void EnsurePostProcessSetup();
	void TeardownPostProcess();
	void ApplyOutlineStyle(FLinearColor Color, float Width);
	void ApplyHighlightToPrimitives(const TArray<UPrimitiveComponent*>& Primitives);
	void InternalHighlightFromRoots(const TArray<AActor*>& Roots, bool bIncludeChildren, FLinearColor Color, float Width);
	TArray<AActor*> FindActorsByTag(FName Tag) const;
	TArray<AActor*> FindActorsByTags(const TArray<FName>& Tags) const;

	UPROPERTY()
	TArray<FHighLightCapturedPrimitiveState> CapturedPrimitiveStates;

	UPROPERTY()
	TObjectPtr<APostProcessVolume> OutlinePostProcessVolume = nullptr;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OutlineMID = nullptr;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> OutlineBaseMaterial = nullptr;

	UPROPERTY()
	FLinearColor HighlightColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f);

	float HighlightWidth = 1.f;

	int32 MaxPrimitivesToHighlight = 0;
};
