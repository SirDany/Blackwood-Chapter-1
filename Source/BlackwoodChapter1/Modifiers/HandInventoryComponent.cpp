
#include "Modifiers/HandInventoryComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Modifiers/PickupComponent.h"

UHandInventoryComponent::UHandInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UCameraComponent* UHandInventoryComponent::GetOwnerCamera() const
{
	AActor* Owner = GetOwner();

	return Owner
		? Owner->FindComponentByClass<UCameraComponent>()
		: nullptr;
}

bool UHandInventoryComponent::PickupActor(AActor* ItemActor)
{
	if (!IsValid(ItemActor) || ItemActor == GetOwner())
	{
		return false;
	}

	UPickupComponent* Pickup =
		ItemActor->FindComponentByClass<UPickupComponent>();

	UCameraComponent* Camera = GetOwnerCamera();

	if (!IsValid(Pickup) || !IsValid(Camera))
	{
		return false;
	}

	if (!IsValid(ItemActor->GetRootComponent()))
	{
		return false;
	}

	if (IsHoldingItem())
	{
		DropItem();

		if (IsHoldingItem())
		{
			return false;
		}
	}

	// Disable physics and restore any detached primitive attachments first.
	Pickup->SetHeldState(true);

	// Attach the actor to the camera only after its physics has been disabled.
	const bool bAttached = ItemActor->AttachToComponent(
		Camera,
		FAttachmentTransformRules::KeepWorldTransform
	);

	if (!bAttached)
	{
		Pickup->SetHeldState(false);
		return false;
	}

	// Apply the configured held position, rotation, and scale.
	ItemActor->SetActorRelativeTransform(
		Pickup->GetHeldRelativeTransform()
	);

	HeldActor = ItemActor;

	OnItemPickedUp.Broadcast(HeldActor);
	OnInventoryChanged.Broadcast();

	return true;
}

void UHandInventoryComponent::UseHeldItem()
{
	if (!IsValid(HeldActor))
	{
		return;
	}

	// Item-specific primary actions will be implemented separately.
	// Using an item does not drop or detach it.
}

void UHandInventoryComponent::DropItem()
{
	if (!IsValid(HeldActor))
	{
		HeldActor = nullptr;
		return;
	}

	AActor* ItemActor = HeldActor.Get();

	UPickupComponent* Pickup =
		ItemActor->FindComponentByClass<UPickupComponent>();

	if (!IsValid(Pickup))
	{
		return;
	}

	const FVector DropPosition = CalculateDropPosition();

	FRotator DropRotation = ItemActor->GetActorRotation();

	if (UCameraComponent* Camera = GetOwnerCamera())
	{
		DropRotation = FRotator(
			0.0f,
			Camera->GetComponentRotation().Yaw,
			0.0f
		);
	}

	// Detach and position the actor before restoring physics.
	ItemActor->DetachFromActor(
		FDetachmentTransformRules::KeepWorldTransform
	);

	ItemActor->SetActorLocationAndRotation(
		DropPosition,
		DropRotation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	Pickup->SetHeldState(false);

	HeldActor = nullptr;

	OnItemDropped.Broadcast(ItemActor);
	OnInventoryChanged.Broadcast();
}

FVector UHandInventoryComponent::CalculateDropPosition() const
{
	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return FVector::ZeroVector;
	}

	UCameraComponent* Camera = GetOwnerCamera();

	if (!IsValid(Camera))
	{
		return Owner->GetActorLocation()
			+ Owner->GetActorForwardVector() * DropDefaultDistance;
	}

	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector CameraForward = Camera->GetForwardVector();

	const FVector TraceEnd =
		CameraLocation + CameraForward * DropPlacementRange;

	FHitResult Hit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.AddIgnoredActor(HeldActor);

	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams))
	{
		const FVector Candidate =
			Hit.ImpactPoint
			+ Hit.ImpactNormal * DropSurfaceOffset;

		if (IsDropPositionSafe(Candidate, DropSafetyRadius))
		{
			return Candidate;
		}

		const FVector RaisedCandidate =
			Candidate
			+ FVector(0.0f, 0.0f, DropSafetyRadius * 2.0f);

		if (IsDropPositionSafe(RaisedCandidate, DropSafetyRadius))
		{
			return RaisedCandidate;
		}
	}

	return CameraLocation
		+ CameraForward * DropDefaultDistance;
}

bool UHandInventoryComponent::IsDropPositionSafe(
	const FVector& Position,
	float Radius) const
{
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return false;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(HeldActor);

	return !World->OverlapBlockingTestByChannel(
		Position,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);
}
