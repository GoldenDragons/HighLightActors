// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HighLightActorsLibrary.generated.h"

class AActor;

/** Blueprint entry points for the current world's outline selection. */
UCLASS()
class HIGHLIGHTACTORS_API UHighLightActorsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Highlight Actor Outline"))
	static void HighlightActor(const UObject* WorldContextObject, AActor* Actor, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Highlight Actor Outlines", AutoCreateRefTerm = "Actors"))
	static void HighlightActors(const UObject* WorldContextObject, const TArray<AActor*>& Actors, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Highlight Actor Outlines By Tag"))
	static void HighlightActorsWithTag(const UObject* WorldContextObject, FName Tag, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Highlight Actor Outlines By Tags", AutoCreateRefTerm = "Tags"))
	static void HighlightActorsWithTags(const UObject* WorldContextObject, const TArray<FName>& Tags, bool bIncludeChildren = true,
		FLinearColor OutlineColor = FLinearColor(0.828f, 0.364f, 0.003f, 1.f), float OutlineWidth = 1.f);

	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Clear Actor Outlines"))
	static void ClearHighlights(const UObject* WorldContextObject);

	/** Zero means no limit. */
	UFUNCTION(BlueprintCallable, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Set Max Outline Primitives"))
	static void SetMaxPrimitivesToHighlight(const UObject* WorldContextObject, int32 MaxCount);

	UFUNCTION(BlueprintPure, Category = "HighLightActors|Highlight",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Get Max Outline Primitives"))
	static int32 GetMaxPrimitivesToHighlight(const UObject* WorldContextObject);
};
