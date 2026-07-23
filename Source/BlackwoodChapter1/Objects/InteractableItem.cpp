// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/InteractableItem.h"

// Sets default values
AInteractableItem::AInteractableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AInteractableItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (ItemData && ItemData->WorldMesh)
    {
        MeshComp->SetStaticMesh(ItemData->WorldMesh); // auto-updates in editor when you assign data
    }
}

void AInteractableItem::Interact_Implementation(AActor* Interactor)
{
    /*if (UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>())
    {
        Inventory->AddItem(ItemData); // inventory now just holds UPickupItemData* pointers
        Destroy();
    }*/
}

FText AInteractableItem::GetInteractionPrompt_Implementation() const
{
    return ItemData ? FText::Format(FText::FromString("Pick up {0}"), ItemData->DisplayName)
        : FText::FromString("Pick up");
}