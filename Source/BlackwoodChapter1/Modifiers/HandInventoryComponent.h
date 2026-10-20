
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandInventoryComponent.generated.h"

class AActor;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnItemPickedUp,
	AActor*,
	ItemActor
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnItemDropped,
	AActor*,
	ItemActor
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UHandInventoryComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UHandInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool PickupActor(AActor* ItemActor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseHeldItem();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	AActor* GetHeldActor() const
	{
		return HeldActor;
	}

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsHoldingItem() const
	{
		return IsValid(HeldActor);
	}

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropPlacementRange = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropSafetyRadius = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropSurfaceOffset = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Drop")
	float DropDefaultDistance = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemPickedUp OnItemPickedUp;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemDropped OnItemDropped;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TObjectPtr<AActor> HeldActor = nullptr;

private:
	UCameraComponent* GetOwnerCamera() const;

	FVector CalculateDropPosition() const;

	bool IsDropPositionSafe(
		const FVector& Position,
		float Radius
	) const;
};
