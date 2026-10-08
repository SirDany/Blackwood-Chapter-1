// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modifiers/InteractableComponent.h"
#include "PickupComponent.generated.h"

class UPickupItemData;
class UHandInventoryComponent;

/**
 * Pickup-specific interaction component
 * Handles interaction with pickupable items
 */
UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UPickupComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UPickupComponent();

	/** Sets the item data for this pickup */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetItemData(UPickupItemData* NewItemData);

	/** Returns the item data */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	UPickupItemData* GetItemData() const { return ItemData; }

protected:
	/** The item data associated with this pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	UPickupItemData* ItemData;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

private:
	/** Attempts to add the item to the interactor's inventory */
	bool TryAddToInventory(AActor* Interactor);
};
