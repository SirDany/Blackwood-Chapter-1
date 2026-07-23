// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InteractableEnemy.h"
#include "Modifiers/StatComponent.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AInteractableEnemy::AInteractableEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInteractableEnemy::BeginPlay()
{
	Super::BeginPlay(); // AStatsCharacter's BeginPlay, if it does its own stat setup

	AngerStat = GetStatByName("Anger"); // AStatsCharacter finds the UStatComponent whose StatName == "Anger"

	if (AngerStat)
	{
		AngerStat->OnStatChanged.AddDynamic(this, &AInteractableEnemy::OnAngerChanged);
		AngerStat->OnStatThresholdReached.AddDynamic(this, &AInteractableEnemy::OnAngerThresholdReached);
		AngerStat->OnStatFull.AddDynamic(this, &AInteractableEnemy::OnAngerFull);
	}
}

void AInteractableEnemy::Interact_Implementation(AActor* Interactor)
{
	if (bIsCarried || !Interactor)
	{
		return;
	}

	// Already carried -> treat interact as "drop" instead of picking up again
	if (bIsCarried)
	{
		Drop();
		return;
	}

	USkeletalMeshComponent* InteractorMesh = Interactor->FindComponentByClass<USkeletalMeshComponent>();
	if (!InteractorMesh || !InteractorMesh->DoesSocketExist(CarrySocketName))
	{
		return;
	}

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->UnPossess();
	}

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	const FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true);

	AttachToComponent(InteractorMesh, AttachRules, CarrySocketName);

	Carrier = Interactor;
	bIsCarried = true;
}

void AInteractableEnemy::Drop()
{
	if (!bIsCarried)
	{
		return;
	}

	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	// NOTE: re-assign/spawn an AIController here depending on how your project
	// normally possesses monsters (auto-possess-on-spawn vs. manual spawn logic).

	Carrier = nullptr;
	bIsCarried = false;
}

bool AInteractableEnemy::TrySootheAnger(float ReductionAmount)
{
	if (!AngerStat || bSootheOnCooldown)
	{
		return false;
	}

	AngerStat->ModifyStat(-FMath::Abs(ReductionAmount));

	bSootheOnCooldown = true;
	GetWorldTimerManager().SetTimer(SootheCooldownHandle, [this]()
		{
			bSootheOnCooldown = false;
		}, SootheCooldownDuration, false);

	return true;
}

void AInteractableEnemy::OnAngerChanged(float NewValue, float Percent)
{
	// Hook for continuous feedback — struggle animation intensity scaling with Percent, etc.
}

void AInteractableEnemy::OnAngerThresholdReached(int32 ThresholdIndex)
{
	// Thresholds array defaults to {0.75, 0.5, 0.25} counting DOWN from max in your component,
	// so double check which direction anger runs (rising vs draining) before relying on index meaning.
}

void AInteractableEnemy::OnAngerFull()
{
	if (bIsCarried)
	{
		Drop(); // anger hit MaxValue while carried -> thrashes free
	}
}

FText AInteractableEnemy::GetInteractionPrompt_Implementation() const
{
	return bIsCarried ? FText::FromString("Drop") : FText::FromString("Pick up");
}