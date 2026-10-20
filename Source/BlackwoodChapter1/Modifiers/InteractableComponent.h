// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IInteractable.h"
#include "InteractableComponent.generated.h"

/**
 * Generic interactable component that can be added to any actor
 * Provides basic interaction prompt and blueprint event for custom behavior
 */
UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UInteractableComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
	UInteractableComponent();

	/** The interaction prompt text displayed to the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt = FText::FromString("Interact");

	/** Blueprint event for custom interaction behavior */
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction", meta = (DisplayName = "On Interact"))
	void BP_OnInteract(AActor* Interactor);

protected:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
};
