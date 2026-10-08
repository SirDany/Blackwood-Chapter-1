#include "Modifiers/HandInventoryComponent.h"

#include "Camera/CameraComponent.h"
#include "Data/PickupItemData.h"
#include "Engine/World.h"
#include "Objects/HeldItem.h"
#include "Modifiers/PickupComponent.h"

UHandInventoryComponent::UHandInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HeldItemData = nullptr;
	HeldItemActor = nullptr;
}

void UHandInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHandInventoryComponent::PickupItem(UPickupItemData* ItemData)
{
	if (!ItemData)
	{
		return;
	}

	// If already holding an item, drop it first.
	if (IsHoldingItem())
	{
		DropItem();
	}

	HeldItemData = ItemData;

	SpawnHeldItemVisual();

	OnItemPickedUp.Broadcast(ItemData);
	OnInventoryChanged.Broadcast();
}

void UHandInventoryComponent::DropItem()
{
	if (!IsHoldingItem())
	{
		return;
	}

	if (!PickupActorClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("HandInventoryComponent: PickupActorClass is not set.")
		);
		return;
	}

	FVector DropPosition = CalculateDropPosition();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* DroppedActor = GetWorld()->SpawnActor<AActor>(
		PickupActorClass,
		DropPosition,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!DroppedActor)
	{
		return;
	}

	UPickupComponent* PickupComponent =
		DroppedActor->FindComponentByClass<UPickupComponent>();

	if (!PickupComponent)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("HandInventoryComponent: PickupActorClass '%s' does not contain a PickupComponent."),
			*GetNameSafe(PickupActorClass)
		);

		DroppedActor->Destroy();
		return;
	}

	UPickupItemData* DroppedItemData = HeldItemData;

	PickupComponent->SetItemData(DroppedItemData);

	DestroyHeldItemVisual();
	HeldItemData = nullptr;

	OnItemDropped.Broadcast();
	OnInventoryChanged.Broadcast();
}

void UHandInventoryComponent::SpawnHeldItemVisual()
{
	if (!HeldItemData)
	{
		return;
	}

	DestroyHeldItemVisual();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	HeldItemActor =
		GetWorld()->SpawnActor<AHeldItem>(
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
		return Owner->GetActorLocation() +
			Owner->GetActorForwardVector() * DropDefaultDistance;
	}

	const FVector CameraLoc = Camera->GetComponentLocation();
	const FVector CameraForward = Camera->GetForwardVector();

	// Trace forward from the player's camera.
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
			HitResult.ImpactPoint +
			HitResult.ImpactNormal * DropSurfaceOffset;

		if (IsDropPositionSafe(DropPos, DropSafetyRadius))
		{
			return DropPos;
		}

		// Try a position slightly higher if the first position
		// is blocked.
		const FVector AdjustedPos =
			DropPos + FVector(0.0f, 0.0f, DropSafetyRadius * 2.0f);

		if (IsDropPositionSafe(AdjustedPos, DropSafetyRadius))
		{
			return AdjustedPos;
		}
	}

	// No usable surface found.
	return CameraLoc + CameraForward * DropDefaultDistance;
}

bool UHandInventoryComponent::IsDropPositionSafe(
	const FVector& Position,
	float Radius)
{
	FCollisionShape Sphere =
		FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	return !GetWorld()->OverlapBlockingTestByChannel(
		Position,
		FQuat::Identity,
		ECC_Visibility,
		Sphere,
		QueryParams
	);
}