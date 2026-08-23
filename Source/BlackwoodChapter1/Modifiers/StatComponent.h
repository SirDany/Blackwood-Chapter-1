// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, float, NewValue, float, Percent); //Event dispatch to signal the stat has changed, sending out the new stat value and a pre calculated 0-1 percentage value
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatThresholdReached, int32, ThersholdIndex); //Event dispatch to indicate the stat has reached a specified threshold corresponding to predefined levels (index) eg: index 2=25% of stat
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatFull);


UCLASS( ClassGroup=(Stat), meta=(BlueprintSpawnableComponent) )
class BLACKWOODCHAPTER1_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

	//////////////////////////Functions//////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ModifyStat(float Delta); // A blueprint callable function that increases (positive delta) or decreases (negative delta) the stats value

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetStatDrainRate(float NewRate); //Blueprint callable function to alter stat darin rate

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetPassiveDrainEnabled(bool bEnabled); //When we need to toggle stat drain on/off (pause menu, cutscenes, safe zones, etc)

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetCurrentValue() const { return CurrentValue; } //Pure node that doesnt require an execution pin in BP graphs. Const correctness ensures no data is modified with this function as it is a simple getter

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxValue() const { return MaxValue; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetPercent() const { return MaxValue > 0.f ? CurrentValue / MaxValue : 0.f; } //Returns the normalized value of the stats percentage to facilitate usage by other systems such as audio or post-process effects.

	UFUNCTION(BlueprintPure, Category = "Stat")
	bool IsDepleted() const { return CurrentValue <= 0.f; } //For convenience to avoid multiple checks throughout callers and keeps code readable

	//////////////////////////Properties//////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Config")
	FName StatName = "Stat";

	UPROPERTY(EditAnywhere, Category = "Stat|Config", meta = (ClampMin = "0.0"))
	float MaxValue = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stat|Config", meta = (ClampMin = "0.0"))
	float StartingValue = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stat|Config", meta = (ClampMin = "0.1"))
	float DrainTickInterval = 0.5f; //How often the drain is activated in seconds, 0.5s is imperceptible to players and cheaper than 60FPS

	UPROPERTY(EditAnywhere, Category = "Stat|Config", meta = (ClampMin = "0.0"))
	float PassiveDrainPerTick = 0.f; //How much is drained per tick, each stat can set it's own value and starts at 0

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stat|Config")
	TArray<float> Thresholds = { 0.75f,0.5f,0.25f }; //configurable thresehold limits for the stat. 

	//////////////////////////Delegate Properties//////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChanged OnStatChanged; //Property to expose the delegates to the blueprints to allow them to be bound with the assign node

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatThresholdReached OnStatThresholdReached;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatDepleted OnStatDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatFull OnStatFull;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	float CurrentValue = 0.f;
	TArray<bool> ThresholdsFired; //Tracks which threshold have already been processed

	FTimerHandle DrainTimerHandle; //A simple timer token to handle the drain ticks

	void TickPassiveDrain();
	void CheckThresholds(float OldValue);
	void StartDrainTimer();
	void StopDrainTimer();



};