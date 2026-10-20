// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/StatsCharacter.h"
#include "Interfaces/IInteractable.h"
#include "InteractableEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWOODCHAPTER1_API AInteractableEnemy : public AStatsCharacter, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractableEnemy();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void Drop();

	UFUNCTION(BlueprintCallable, Category = "Anger")
	bool TrySootheAnger(float ReductionAmount);

	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsCarried() const { return bIsCarried; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAngerChanged(float NewValue, float Percent);

	UFUNCTION()
	void OnAngerThresholdReached(int32 ThresholdIndex);

	UFUNCTION()
	void OnAngerFull();

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FName CarrySocketName = "HandSocket_R";

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	bool bIsCarried = false;

	UPROPERTY()
	TObjectPtr<AActor> Carrier;

	UPROPERTY(EditDefaultsOnly, Category = "Anger")
	float SootheCooldownDuration = 3.0f;

	bool bSootheOnCooldown = false;
	FTimerHandle SootheCooldownHandle;

	UPROPERTY()
	TObjectPtr<UStatComponent> AngerStat;
	
};
