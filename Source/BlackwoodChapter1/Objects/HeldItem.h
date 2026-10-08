// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeldItem.generated.h"

class UPickupItemData;
class UStaticMeshComponent;

/**
 * Visual representation of an item being held or dropped
 * Can be either in-hand (attached to camera) or in-world (dropped)
 */
UCLASS()
class BLACKWOODCHAPTER1_API AHeldItem : public AActor
{
	GENERATED_BODY()

public:
	AHeldItem();

	/** Sets the item data and updates the visual mesh */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemData(UPickupItemData* NewItemData);

	/** Returns the current item data */
	UFUNCTION(BlueprintPure, Category = "Item")
	UPickupItemData* GetItemData() const { return ItemData; }

	/** Attaches this item to the camera/hand of an actor (for holding) */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void AttachToCamera(AActor* OwnerActor);

	/** Detaches from camera and enables physics for dropping */
	UFUNCTION(BlueprintCallable, Category = "Item")
	void DetachAndDrop();

protected:
	/** The mesh component for this item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	/** The item data this actor represents */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UPickupItemData* ItemData;

	virtual void BeginPlay() override;

private:
	/** Time before a dropped item is destroyed if it doesn't interact with anything */
	static constexpr float DROPPED_ITEM_LIFETIME = 300.0f;
};
