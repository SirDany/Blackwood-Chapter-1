
#include "Player/PlayerStatsCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Modifiers/HandInventoryComponent.h"
#include "BlackwoodChapter1.h"

APlayerStatsCharacter::APlayerStatsCharacter()
{
	InteractionComp =
		CreateDefaultSubobject<UInteractionComponent>(
			TEXT("InteractionComponent"));

	InventoryComp =
		CreateDefaultSubobject<UHandInventoryComponent>(
			TEXT("HandInventoryComponent"));
}

void APlayerStatsCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC =
		Cast<APlayerController>(GetController());

	if (!PC || !DefaultMappingContext)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();

	if (!LocalPlayer)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void APlayerStatsCharacter::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInput)
	{
		UE_LOG(
			LogBlackwoodChapter1,
			Error,
			TEXT("Failed to find Enhanced Input Component on '%s'."),
			*GetNameSafe(this));

		return;
	}

	if (InteractAction)
	{
		EnhancedInput->BindAction(
			InteractAction,
			ETriggerEvent::Started,
			this,
			&APlayerStatsCharacter::OnInteractTriggered);
	}

	if (PrimaryAction)
	{
		EnhancedInput->BindAction(
			PrimaryAction,
			ETriggerEvent::Started,
			this,
			&APlayerStatsCharacter::OnPrimaryActionTriggered);
	}

	if (SecondaryAction)
	{
		EnhancedInput->BindAction(
			SecondaryAction,
			ETriggerEvent::Started,
			this,
			&APlayerStatsCharacter::OnSecondaryActionTriggered);
	}
}

void APlayerStatsCharacter::OnInteractTriggered(
	const FInputActionValue& Value)
{
	if (InteractionComp)
	{
		InteractionComp->TryInteract();
	}
}

void APlayerStatsCharacter::OnPrimaryActionTriggered(
	const FInputActionValue& Value)
{
	if (InventoryComp)
	{
		InventoryComp->UseHeldItem();
	}
}

void APlayerStatsCharacter::OnSecondaryActionTriggered(
	const FInputActionValue& Value)
{
	if (InventoryComp)
	{
		InventoryComp->DropItem();
	}
}
