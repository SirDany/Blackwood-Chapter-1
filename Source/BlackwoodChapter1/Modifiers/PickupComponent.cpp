
#include "Modifiers/PickupComponent.h"

#include "Data/PickupItemData.h"
#include "GameFramework/Actor.h"
#include "Modifiers/HandInventoryComponent.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ItemData = nullptr;
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
		if (AActor* Owner = GetOwner())
		{
			Owner->Destroy();
		}
	}
}

FText UPickupComponent::GetInteractionPrompt_Implementation() const
{
	if (ItemData)
	{
		return FText::Format(
			FText::FromString("Pick up {0}"),
			ItemData->DisplayName
		);
	}

	return InteractionPrompt;
}

bool UPickupComponent::TryAddToInventory(AActor* Interactor)
{
	if (!ItemData || !Interactor)
	{
		return false;
	}

	AActor* PickupActor = GetOwner();

	if (!PickupActor)
	{
		return false;
	}

	UHandInventoryComponent* Inventory =
		Interactor->FindComponentByClass<UHandInventoryComponent>();

	if (!Inventory)
	{
		return false;
	}

	// Pass the actual class of the actor being picked up.
	return Inventory->PickupItem(
		ItemData,
		PickupActor->GetClass()
	);
}
