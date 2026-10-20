
#include "Modifiers/PickupComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Modifiers/HandInventoryComponent.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InteractionPrompt = FText::FromString("Pick up");
}

void UPickupComponent::Interact_Implementation(AActor* Interactor)
{
	TryPickup(Interactor);
}

FText UPickupComponent::GetInteractionPrompt_Implementation() const
{
	return FText::Format(
		FText::FromString("Pick up {0} [E]"),
		DisplayName
	);
}

bool UPickupComponent::TryPickup(AActor* Interactor)
{
	if (!IsValid(Interactor))
	{
		return false;
	}

	AActor* ItemActor = GetOwner();

	if (!IsValid(ItemActor))
	{
		return false;
	}

	UHandInventoryComponent* Inventory =
		Interactor->FindComponentByClass<UHandInventoryComponent>();

	if (!IsValid(Inventory))
	{
		return false;
	}

	return Inventory->PickupActor(ItemActor);
}

void UPickupComponent::SetHeldState(bool bHeld)
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	USceneComponent* Root = Owner->GetRootComponent();

	if (!IsValid(Root))
	{
		return;
	}

	if (bHeld)
	{
		if (bHasSavedState)
		{
			return;
		}

		OriginalWorldScale = Owner->GetActorScale3D();
		bPreviousActorCollisionEnabled = Owner->GetActorEnableCollision();

		SavedPrimitiveStates.Reset();

		TArray<UPrimitiveComponent*> PrimitiveComponents;
		Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

		// Save the original collision and physics states before modifying anything.
		for (UPrimitiveComponent* Component : PrimitiveComponents)
		{
			if (!IsValid(Component))
			{
				continue;
			}

			FPickupPrimitiveState State;
			State.Component = Component;
			State.CollisionEnabled = Component->GetCollisionEnabled();
			State.bSimulatePhysics = Component->IsSimulatingPhysics();

			SavedPrimitiveStates.Add(State);
		}

		// Disable physics before attempting to restore component attachments.
		for (const FPickupPrimitiveState& State : SavedPrimitiveStates)
		{
			UPrimitiveComponent* Component = State.Component;

			if (!IsValid(Component))
			{
				continue;
			}

			if (Component->IsSimulatingPhysics())
			{
				Component->SetSimulatePhysics(false);
			}
		}

		// Physics simulation can detach a primitive from its parent.
		// Reattach detached non-root primitives while preserving their world transforms.
		for (const FPickupPrimitiveState& State : SavedPrimitiveStates)
		{
			UPrimitiveComponent* Component = State.Component;

			if (!IsValid(Component) || Component == Root)
			{
				continue;
			}

			if (Component->GetAttachParent() == nullptr)
			{
				Component->AttachToComponent(
					Root,
					FAttachmentTransformRules::KeepWorldTransform
				);
			}
		}

		// Disable component collision and actor collision while held.
		for (const FPickupPrimitiveState& State : SavedPrimitiveStates)
		{
			if (IsValid(State.Component))
			{
				State.Component->SetCollisionEnabled(
					ECollisionEnabled::NoCollision
				);
			}
		}

		Owner->SetActorEnableCollision(false);
		bHasSavedState = true;

		return;
	}

	if (!bHasSavedState)
	{
		return;
	}

	// Restore the actor's original scale and collision setting.
	Owner->SetActorScale3D(OriginalWorldScale);
	Owner->SetActorEnableCollision(bPreviousActorCollisionEnabled);

	// Restore component collision settings.
	for (const FPickupPrimitiveState& State : SavedPrimitiveStates)
	{
		if (IsValid(State.Component))
		{
			State.Component->SetCollisionEnabled(State.CollisionEnabled);
		}
	}

	// Restore physics only after the inventory has detached and positioned the actor.
	for (const FPickupPrimitiveState& State : SavedPrimitiveStates)
	{
		if (IsValid(State.Component))
		{
			State.Component->SetSimulatePhysics(State.bSimulatePhysics);
		}
	}

	SavedPrimitiveStates.Reset();
	bHasSavedState = false;
}

FTransform UPickupComponent::GetHeldRelativeTransform() const
{
	FTransform Result = HeldRelativeTransform;

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return Result;
	}

	FVector HeldScale;

	if (bUseAutomaticHeldScale)
	{
		FVector BoundsOrigin;
		FVector BoundsExtent;

		Owner->GetActorBounds(false, BoundsOrigin, BoundsExtent);

		const float CurrentLargestDimension =
			2.0f * FMath::Max3(
				BoundsExtent.X,
				BoundsExtent.Y,
				BoundsExtent.Z
			);

		if (CurrentLargestDimension > KINDA_SMALL_NUMBER)
		{
			const float ScaleFactor =
				TargetHeldSize / CurrentLargestDimension;

			HeldScale = OriginalWorldScale * ScaleFactor;
		}
		else
		{
			HeldScale =
				OriginalWorldScale * HeldScaleMultiplier;
		}
	}
	else
	{
		HeldScale =
			OriginalWorldScale * HeldScaleMultiplier;
	}

	Result.SetScale3D(HeldScale);

	return Result;
}
