
#include "Modifiers/HandInventoryComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/PickupItemData.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Objects/HeldItem.h"
#include "Modifiers/PickupComponent.h"

UHandInventoryComponent::UHandInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HeldItemData = nullptr;
	HeldItemActor = nullptr;
	HeldActorClass = nullptr;
}

void UHandInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UHandInventoryComponent::PickupItem(
	UPickupItemData* ItemData,
	TSubclassOf<AActor> ActorClass)
{
	if (!ItemData || !ActorClass)
	{
		return false;
	}

	// Replace the currently held item if necessary.
	if (IsHoldingItem())
	{
		DropItem();

		// If dropping failed, keep the existing item.
		if (IsHoldingItem())
		{
			return false;
		}
	}

	HeldItemData = ItemData;
	HeldActorClass = ActorClass;

	SpawnHeldItemVisual();

	OnItemPickedUp.Broadcast(ItemData);
	OnInventoryChanged.Broadcast();

	return true;
}

void UHandInventoryComponent::UseHeldItem()
{
	// Intentionally empty until item-specific primary actions are implemented.
}

void UHandInventoryComponent::DropItem()
{
	if (!IsHoldingItem())
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World || !HeldItemData || !HeldActorClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("HandInventoryComponent: Cannot drop item because its world, data, or actor class is missing.")
		);
		return;
	}

	const FVector DropPosition = CalculateDropPosition();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Recreate the original actor class.
	AActor* DroppedActor = World->SpawnActor<AActor>(
		HeldActorClass,
		DropPosition,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!DroppedActor)
	{
		return;
	}

	// The recreated actor must support pickup interaction.
	UPickupComponent* PickupComponent =
		DroppedActor->FindComponentByClass<UPickupComponent>();

	if (!PickupComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("HandInventoryComponent: Spawned class '%s' has no PickupComponent."),
			*GetNameSafe(HeldActorClass)
		);

		DroppedActor->Destroy();
		return;
	}

	// Restore the item's definition.
	PickupComponent->SetItemData(HeldItemData);

	// Clear inventory state only after successful spawning and setup.
	DestroyHeldItemVisual();

	HeldItemData = nullptr;
	HeldActorClass = nullptr;

	OnItemDropped.Broadcast();
	OnInventoryChanged.Broadcast();
}

void UHandInventoryComponent::SpawnHeldItemVisual()
{
	if (!HeldItemData || !GetWorld())
	{
		return;
	}

	DestroyHeldItemVisual();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	HeldItemActor = GetWorld()->SpawnActor<AHeldItem>(
		AHeldItem::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (HeldItemActor)
	{
		HeldItemActor->SetItemData(HeldItemData);
		HeldItemActor->AttachToCamera(GetOwner());
	}
}

void UHandInventoryComponent::DestroyHeldItemVisual()
{
	if (HeldItemActor)
	{
		HeldItemActor->Destroy();
		HeldItemActor = nullptr;
	}
}

FVector UHandInventoryComponent::CalculateDropPosition()
{
	AActor* Owner = GetOwner();

	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	UCameraComponent* Camera =
		Owner->FindComponentByClass<UCameraComponent>();

	if (!Camera)
	{
		return Owner->GetActorLocation()
			+ Owner->GetActorForwardVector() * DropDefaultDistance;
	}

	const FVector CameraLoc = Camera->GetComponentLocation();
	const FVector CameraForward = Camera->GetForwardVector();

	FHitResult HitResult;

	const FVector RayEnd =
		CameraLoc + CameraForward * DropPlacementRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLoc,
		RayEnd,
		ECC_Visibility,
		QueryParams
	);

	if (bHit)
	{
		const FVector DropPos =
			HitResult.ImpactPoint
			+ HitResult.ImpactNormal * DropSurfaceOffset;

		if (IsDropPositionSafe(DropPos, DropSafetyRadius))
		{
			return DropPos;
		}

		const FVector AdjustedPos =
			DropPos
			+ FVector(0.0f, 0.0f, DropSafetyRadius * 2.0f);

		if (IsDropPositionSafe(AdjustedPos, DropSafetyRadius))
		{
			return AdjustedPos;
		}
	}

	return CameraLoc + CameraForward * DropDefaultDistance;
}

bool UHandInventoryComponent::IsDropPositionSafe(
	const FVector& Position,
	float Radius) const
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return false;
	}

	const FCollisionShape Sphere =
		FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	return !World->OverlapBlockingTestByChannel(
		Position,
		FQuat::Identity,
		ECC_Visibility,
		Sphere,
		QueryParams
	);
}
