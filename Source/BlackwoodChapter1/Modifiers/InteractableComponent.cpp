// Fill out your copyright notice in the Description page of Project Settings.

#include "Modifiers/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::Interact_Implementation(AActor* Interactor)
{
	// Call the blueprint event
	BP_OnInteract(Interactor);
}

FText UInteractableComponent::GetInteractionPrompt_Implementation() const
{
	return InteractionPrompt;
}
