// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/StatsCharacter.h"

// Sets default values
AStatsCharacter::AStatsCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	SanityComponent = CreateDefaultSubobject<UStatComponent>(TEXT("SanityComponent"));
	SanityComponent->StatName = FName("Sanity");

}

// Called when the game starts or when spawned
void AStatsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AStatsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//Allows BP to search for stats by name
UStatComponent* AStatsCharacter::GetStatByName(FName StatName) const
{
	TArray<UStatComponent*> StatComponents;
	GetComponents<UStatComponent>(StatComponents);

	for (UStatComponent* Comp : StatComponents)
	{
		if (Comp && Comp->StatName == StatName)
		{
			return Comp;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GetStatsByName: No stat found with name %s on %s"), *StatName.ToString(), *GetName());
	return nullptr;
}

