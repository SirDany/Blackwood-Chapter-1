// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "Player/StatsCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Modifiers/InteractionComponent.h"
#include "PlayerStatsCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UInteractionComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
struct FInputActionValue;

//DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 * 
 */
UCLASS()
class BLACKWOODCHAPTER1_API APlayerStatsCharacter : public AStatsCharacter
{
	GENERATED_BODY()

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

	void OnInteractTriggered(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TObjectPtr<UInteractionComponent> InteractionComp;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;
	
};
