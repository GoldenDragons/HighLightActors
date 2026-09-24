// Copyright Epic Games, Inc. All Rights Reserved.

#include "HighLightActorGather.h"

#include "Components/ChildActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

namespace HighLightActorGather
{
	void CollectActorHierarchy(AActor* Root, TArray<AActor*>& OutActors, bool bIncludeChildren)
	{
		if (!IsValid(Root))
		{
			return;
		}

		if (!bIncludeChildren)
		{
			OutActors.AddUnique(Root);
			return;
		}

		TArray<AActor*> Stack;
		Stack.Push(Root);

		while (Stack.Num() > 0)
		{
			AActor* Current = Stack.Pop(EAllowShrinking::No);
			if (!IsValid(Current) || OutActors.Contains(Current))
			{
				continue;
			}

			OutActors.Add(Current);

			TArray<AActor*> DirectChildren;
			Current->GetAttachedActors(DirectChildren, /*bResetArray=*/true, /*bRecursivelyIncludeAttachedActors=*/false);
			for (AActor* Attached : DirectChildren)
			{
				if (IsValid(Attached))
				{
					Stack.Push(Attached);
				}
			}

			TArray<UChildActorComponent*> ChildActorComps;
			Current->GetComponents(ChildActorComps);
			for (UChildActorComponent* Cac : ChildActorComps)
			{
				if (IsValid(Cac))
				{
					if (AActor* ChildActor = Cac->GetChildActor())
					{
						if (IsValid(ChildActor))
						{
							Stack.Push(ChildActor);
						}
					}
				}
			}
		}
	}

	void CollectPrimitivesFromActors(const TArray<AActor*>& Actors, TArray<UPrimitiveComponent*>& OutPrimitives)
	{
		for (AActor* Actor : Actors)
		{
			if (!IsValid(Actor))
			{
				continue;
			}

			TArray<UPrimitiveComponent*> Primitives;
			Actor->GetComponents<UPrimitiveComponent>(Primitives);
			for (UPrimitiveComponent* Prim : Primitives)
			{
				if (IsValid(Prim) && Prim->IsRegistered())
				{
					OutPrimitives.Add(Prim);
				}
			}
		}
	}
}
