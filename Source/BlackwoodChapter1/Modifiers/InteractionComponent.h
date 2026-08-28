// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class BLACKWOODCHAPTER1_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractRange = 200.f;

protected:
	FVector GetOwnerViewLocation() const;
	FRotator GetOwnerViewRotation() const;
};
