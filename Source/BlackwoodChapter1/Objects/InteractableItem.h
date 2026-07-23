// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/PickupItemData.h"
#include "Interfaces/IInteractable.h"
#include "InteractableItem.generated.h"

UCLASS()
class BLACKWOODCHAPTER1_API AInteractableItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
    AInteractableItem();

    UPROPERTY(EditAnywhere, Category = "Item")
    UPickupItemData* ItemData;

    virtual void Interact_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;

    virtual void OnConstruction(const FTransform& Transform) override;

};
