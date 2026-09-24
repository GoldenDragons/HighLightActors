// Copyright Epic Games, Inc. All Rights Reserved.

#include "HighLightActorsLibrary.h"

#include "HighLightActorsSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

namespace
{
UHighLightActorsSubsystem* GetHighlightSubsystem(const UObject* WorldContextObject)
{
	if (!GEngine || !IsValid(WorldContextObject))
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	return World ? World->GetSubsystem<UHighLightActorsSubsystem>() : nullptr;
}
}

void UHighLightActorsLibrary::HighlightActor(const UObject* WorldContextObject, AActor* Actor, bool bIncludeChildren,
	FLinearColor OutlineColor, float OutlineWidth)
{
	if (UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		Subsystem->HighlightActor(Actor, bIncludeChildren, OutlineColor, OutlineWidth);
	}
}

void UHighLightActorsLibrary::HighlightActors(const UObject* WorldContextObject, const TArray<AActor*>& Actors,
	bool bIncludeChildren, FLinearColor OutlineColor, float OutlineWidth)
{
	if (UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		Subsystem->HighlightActors(Actors, bIncludeChildren, OutlineColor, OutlineWidth);
	}
}

void UHighLightActorsLibrary::HighlightActorsWithTag(const UObject* WorldContextObject, FName Tag,
	bool bIncludeChildren, FLinearColor OutlineColor, float OutlineWidth)
{
	if (UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		Subsystem->HighlightActorsWithTag(Tag, bIncludeChildren, OutlineColor, OutlineWidth);
	}
}

void UHighLightActorsLibrary::HighlightActorsWithTags(const UObject* WorldContextObject, const TArray<FName>& Tags,
	bool bIncludeChildren, FLinearColor OutlineColor, float OutlineWidth)
{
	if (UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		Subsystem->HighlightActorsWithTags(Tags, bIncludeChildren, OutlineColor, OutlineWidth);
	}
}

void UHighLightActorsLibrary::ClearHighlights(const UObject* WorldContextObject)
{
	if (UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		Subsystem->ClearHighlights();
	}
}

void UHighLightActorsLibrary::SetMaxPrimitivesToHighlight(const UObject* WorldContextObject, int32 MaxCount)
{
	if (UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		Subsystem->SetMaxPrimitivesToHighlight(MaxCount);
	}
}

int32 UHighLightActorsLibrary::GetMaxPrimitivesToHighlight(const UObject* WorldContextObject)
{
	if (const UHighLightActorsSubsystem* Subsystem = GetHighlightSubsystem(WorldContextObject))
	{
		return Subsystem->GetMaxPrimitivesToHighlight();
	}
	return 0;
}
