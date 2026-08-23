// Fill out your copyright notice in the Description page of Project Settings.


#include "Modifiers/StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentValue = FMath::Clamp(StartingValue, 0.f, MaxValue);
	ThresholdsFired.Init(false, Thresholds.Num());

	if (PassiveDrainPerTick > 0.f)
	{
		StartDrainTimer();
	}
	
}

void UStatComponent::ModifyStat(float Delta)
{
	if (FMath::IsNearlyZero(Delta)) //avoids broadcasting events for meaningless, minor changes
	{
		return;
	}

	const float OldValue = CurrentValue;
	CurrentValue = FMath::Clamp(CurrentValue + Delta, 0.f, MaxValue); //safe guard that the value does not go beyong the specified thresholds

	if (FMath::IsNearlyEqual(OldValue, CurrentValue)) //values are essentially the same, nothing to do here, carry on
	{
		return;
	}

	CheckThresholds(OldValue);
	OnStatChanged.Broadcast(CurrentValue, GetPercent()); //check if any thresholds had been reached then broadcast the changed stat to any listeners

	if (CurrentValue <= 0.f)
	{
		OnStatDepleted.Broadcast();
	}
	else if (CurrentValue >= MaxValue)
	{
		OnStatFull.Broadcast();
	}

}

void UStatComponent::CheckThresholds(float OldValue)
{
	const float Percent = GetPercent();
	for (int32 i = 0; i < Thresholds.Num(); ++i)
	{
		if (!ThresholdsFired[i] && Percent <= Thresholds[i])
		{
			ThresholdsFired[i] = true;
			OnStatThresholdReached.Broadcast(i);
		}
		else if (ThresholdsFired[i] && Percent > Thresholds[i])
		{
			ThresholdsFired[i] = false;
		}
	}
}

void UStatComponent::SetStatDrainRate(float NewRate)
{
	PassiveDrainPerTick = FMath::Max(0.f, NewRate); //stops passive regeneration with negative values, can be added in with a regenPerTick 
	PassiveDrainPerTick > 0.f ? StartDrainTimer() : StopDrainTimer();
}

void UStatComponent::SetPassiveDrainEnabled(bool bEnabled)
{
	bEnabled ? StartDrainTimer() : StopDrainTimer();

}

void UStatComponent::TickPassiveDrain()
{
	ModifyStat(-PassiveDrainPerTick);
}

void UStatComponent::StartDrainTimer()
{
	if (!DrainTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DrainTimerHandle, //handle to store the time ID in
			this, //Object that owns the callback
			&UStatComponent::TickPassiveDrain, //The function to call, using a memeber function pointer to ensure type safety
			DrainTickInterval, //Interval in seconds
			true); //true=looping timer
	}
}

void UStatComponent::StopDrainTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(DrainTimerHandle);
}


