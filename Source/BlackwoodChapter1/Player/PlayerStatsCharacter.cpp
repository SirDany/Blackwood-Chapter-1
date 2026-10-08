// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerStatsCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputActionValue.h"
#include "BlackwoodChapter1.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Modifiers/HandInventoryComponent.h"

APlayerStatsCharacter::APlayerStatsCharacter()
{
	InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InventoryComp = CreateDefaultSubobject<UHandInventoryComponent>(TEXT("HandInventoryComponent"));
}

void APlayerStatsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerStatsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerStatsCharacter::OnInteractTriggered);
		EnhancedInput->BindAction(DropAction, ETriggerEvent::Started, this, &APlayerStatsCharacter::OnDropTriggered);
	}
	else
	{
		UE_LOG(LogBlackwoodChapter1, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void APlayerStatsCharacter::OnInteractTriggered(const FInputActionValue& Value)
{
	if (InteractionComp)
	{
		InteractionComp->TryInteract();
	}
}

void APlayerStatsCharacter::OnDropTriggered(const FInputActionValue& Value)
{
	if (InventoryComp)
	{
		InventoryComp->DropItem();
	}
}
