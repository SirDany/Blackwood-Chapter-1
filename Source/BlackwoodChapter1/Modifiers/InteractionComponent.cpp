#include "Modifiers/InteractionComponent.h"

#include "Widgets/InteractionPromptWidget.h"
#include "Interfaces/IInteractable.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!InteractionPromptWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Interaction prompt widget class is not assigned.")
		);
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* PC = OwnerPawn
		? Cast<APlayerController>(OwnerPawn->GetController())
		: nullptr;

	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	InteractionPromptWidget = CreateWidget<UInteractionPromptWidget>(
		PC,
		InteractionPromptWidgetClass
	);

	if (InteractionPromptWidget)
	{
		InteractionPromptWidget->AddToViewport();
		InteractionPromptWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

FVector UInteractionComponent::GetOwnerViewLocation() const
{
	if (const UCameraComponent* Cam =
		GetOwner()->FindComponentByClass<UCameraComponent>())
	{
		return Cam->GetComponentLocation();
	}

	return GetOwner()->GetActorLocation();
}

FRotator UInteractionComponent::GetOwnerViewRotation() const
{
	if (const UCameraComponent* Cam =
		GetOwner()->FindComponentByClass<UCameraComponent>())
	{
		return Cam->GetComponentRotation();
	}

	return GetOwner()->GetActorRotation();
}

void UInteractionComponent::TryInteract()
{
	if (!GetWorld())
	{
		return;
	}

	FHitResult Hit;
	const FVector Start = GetOwnerViewLocation();
	const FVector End =
		Start + GetOwnerViewRotation().Vector() * InteractRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	if (!GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, QueryParams))
	{
		return;
	}

	AActor* HitActor = Hit.GetActor();

	if (!HitActor)
	{
		return;
	}

	if (HitActor->Implements<UInteractable>())
	{
		IInteractable::Execute_Interact(HitActor, GetOwner());
		return;
	}

	TArray<UActorComponent*> Components =
		HitActor->GetComponents().Array();

	for (UActorComponent* Component : Components)
	{
		if (Component && Component->Implements<UInteractable>())
		{
			IInteractable::Execute_Interact(Component, GetOwner());
			return;
		}
	}
}

void UInteractionComponent::UpdateInteractionPrompt()
{
	if (!InteractionPromptWidget || !GetWorld())
	{
		return;
	}

	FHitResult Hit;
	const FVector Start = GetOwnerViewLocation();
	const FVector End =
		Start + GetOwnerViewRotation().Vector() * InteractRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	bool bFoundInteractable = false;
	FText InteractionText;

	if (GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor)
		{
			if (HitActor->Implements<UInteractable>())
			{
				InteractionText =
					IInteractable::Execute_GetInteractionPrompt(HitActor);

				bFoundInteractable = true;
			}
			else
			{
				TArray<UActorComponent*> Components =
					HitActor->GetComponents().Array();

				for (UActorComponent* Component : Components)
				{
					if (Component &&
						Component->Implements<UInteractable>())
					{
						InteractionText =
							IInteractable::Execute_GetInteractionPrompt(
								Component
							);

						bFoundInteractable = true;
						break;
					}
				}
			}
		}
	}

	if (bFoundInteractable)
	{
		InteractionPromptWidget->SetPromptText(InteractionText);
		InteractionPromptWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractionPromptWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInteractionComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateInteractionPrompt();
}