// Fill out your copyright notice in the Description page of Project Settings.

#include "Modifiers/PickupComponent.h"
#include "Data/PickupItemData.h"
#include "Modifiers/HandInventoryComponent.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ItemData = nullptr;

	// Set default interaction prompt for pickups
	InteractionPrompt = FText::FromString("Pick up");
}

void UPickupComponent::SetItemData(UPickupItemData* NewItemData)
{
	ItemData = NewItemData;
}

void UPickupComponent::Interact_Implementation(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	if (TryAddToInventory(Interactor))
	{
		// Destroy the pickup actor after successful pickup
		AActor* Owner = GetOwner();
		if (Owner)
		{
			Owner->Destroy();
		}
	}
}

FText UPickupComponent::GetInteractionPrompt_Implementation() const
{
	if (ItemData)
	{
		return FText::Format(FText::FromString("Pick up {0}"), ItemData->DisplayName);
	}

	return InteractionPrompt;
}

bool UPickupComponent::TryAddToInventory(AActor* Interactor)
{
	if (!ItemData)
	{
		return false;
	}

	// Find the hand inventory component on the interactor
	UHandInventoryComponent* Inventory = Interactor->FindComponentByClass<UHandInventoryComponent>();

	if (Inventory)
	{
		Inventory->PickupItem(ItemData);
		return true;
	}

	return false;
}
