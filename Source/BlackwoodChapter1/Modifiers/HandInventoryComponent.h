
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandInventoryComponent.generated.h"

class UPickupItemData;
class AHeldItem;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnItemPickedUp,
	UPickupItemData*,
	ItemData
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDropped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UHandInventoryComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UHandInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool PickupItem(
		UPickupItemData* ItemData,
		TSubclassOf<AActor> ActorClass
	);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseHeldItem();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPickupItemData* GetHeldItem() const
	{
		return HeldItemData;
	}

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsHoldingItem() const
	{
		return HeldItemData != nullptr;
	}

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropPlacementRange = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropSafetyRadius = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropSurfaceOffset = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropDefaultDistance = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemPickedUp OnItemPickedUp;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemDropped OnItemDropped;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UPickupItemData> HeldItemData;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<AHeldItem> HeldItemActor;

	// The class of the original world actor being held.
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TSubclassOf<AActor> HeldActorClass;

	virtual void BeginPlay() override;

private:
	void SpawnHeldItemVisual();
	void DestroyHeldItemVisual();

	FVector CalculateDropPosition();

	bool IsDropPositionSafe(
		const FVector& Position,
		float Radius
	) const;
};
