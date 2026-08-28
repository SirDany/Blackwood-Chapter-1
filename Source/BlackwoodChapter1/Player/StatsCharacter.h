// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Modifiers/StatComponent.h"
#include "BlackwoodChapter1Character.h"
#include "StatsCharacter.generated.h"

UCLASS()
class BLACKWOODCHAPTER1_API AStatsCharacter : public ABlackwoodChapter1Character
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStatsCharacter();

	///////////////////////Player stats //////////////////////////////////////////

	UFUNCTION(BlueprintPure, Category = "Stats")
	UStatComponent* GetSanity() const { return SanityComponent; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	UStatComponent* GetStatByName(FName StatName) const;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowedPrivateAccess = "true"))
	TObjectPtr<UStatComponent> SanityComponent;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};