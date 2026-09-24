// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"

class AActor;
class UPrimitiveComponent;

namespace HighLightActorGather
{
	/** Collect Root plus all attached actors (recursive). */
	void CollectActorHierarchy(AActor* Root, TArray<AActor*>& OutActors, bool bIncludeChildren);

	/** Collect primitive components from the given actors (each actor: all UPrimitiveComponent). */
	void CollectPrimitivesFromActors(const TArray<AActor*>& Actors, TArray<UPrimitiveComponent*>& OutPrimitives);
}
