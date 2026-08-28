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

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<UInteractable>())
			{
				IInteractable::Execute_Interact(HitActor, GetOwner());
			}
		}
	}
}