// Fill out your copyright notice in the Description page of Project Settings.


#include "Modifiers/InteractionComponent.h"

#include "Interfaces/IInteractable.h"
#include "Camera/CameraComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FVector UInteractionComponent::GetOwnerViewLocation() const
{
	if (const UCameraComponent* Cam = GetOwner()->FindComponentByClass<UCameraComponent>())
	{
		return Cam->GetComponentLocation();
	}
	return GetOwner()->GetActorLocation();
}

FRotator UInteractionComponent::GetOwnerViewRotation() const
{
	if (const UCameraComponent* Cam = GetOwner()->FindComponentByClass<UCameraComponent>())
	{
		return Cam->GetComponentRotation();
	}
	return GetOwner()->GetActorRotation();
}

void UInteractionComponent::TryInteract()
{
	FHitResult Hit;
	const FVector Start = GetOwnerViewLocation();
	const FVector End = Start + GetOwnerViewRotation().Vector() * InteractRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor)
		{
			return;
		}

		// First: allow the actor itself to implement IInteractable.
		if (HitActor->Implements<UInteractable>())
		{
			IInteractable::Execute_Interact(HitActor, GetOwner());
			return;
		}

		// Otherwise look for an interactable component on the actor.
		TArray<UActorComponent*> Components = HitActor->GetComponents().Array();

		for (UActorComponent* Component : Components)
		{
			if (Component && Component->Implements<UInteractable>())
			{
				IInteractable::Execute_Interact(Component, GetOwner());
				return;
			}
		}
	}
}