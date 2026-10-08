// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandInventoryComponent.generated.h"

class UPickupItemData;
class AHeldItem;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemPickedUp, UPickupItemData*, ItemData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDropped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/**
 * Limited inventory system for holding a single item in hand
 * Manages pickup, drop, and visual representation of held items
 */
UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UHandInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHandInventoryComponent();

	/** Picks up an item and holds it */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickupItem(UPickupItemData* ItemData);

	/** Drops the currently held item */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem();

	/** Returns the currently held item, or nullptr if empty */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPickupItemData* GetHeldItem() const { return HeldItemData; }

	/** Returns whether inventory is holding an item */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsHoldingItem() const { return HeldItemData != nullptr; }

	UPROPERTY(EditAnywhere, Category = "Drop")
	TSubclassOf<AActor> PickupActorClass;

	/** Maximum distance for smart drop placement raycast */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropPlacementRange = 500.0f;

	/** Minimum space required around drop location to spawn item safely */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropSafetyRadius = 30.0f;

	/** Offset from surface when dropping on detected surface */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropSurfaceOffset = 50.0f;

	/** Offset in front of player when no surface is detected */
	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropDefaultDistance = 100.0f;

public:
	/** Delegate called when item is picked up */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemPickedUp OnItemPickedUp;

	/** Delegate called when item is dropped */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemDropped OnItemDropped;

	/** Delegate called when inventory changes */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

protected:
	/** The currently held item data */
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	UPickupItemData* HeldItemData;

	/** The actor representing the held item in world */
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	AHeldItem* HeldItemActor;

	virtual void BeginPlay() override;

private:
	/** Spawns the visual representation of the held item */
	void SpawnHeldItemVisual();

	/** Destroys the held item actor */
	void DestroyHeldItemVisual();

	/** Calculates the best drop position for the item */
	FVector CalculateDropPosition();

	/** Checks if a position is safe for dropping (no overlapping geometry) */
	bool IsDropPositionSafe(const FVector& Position, float Radius);
};
