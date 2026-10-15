
#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Player/StatsCharacter.h"
#include "Modifiers/InteractionComponent.h"
#include "PlayerStatsCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UHandInventoryComponent;
class UInputComponent;

UCLASS()
class BLACKWOODCHAPTER1_API APlayerStatsCharacter : public AStatsCharacter
{
	GENERATED_BODY()

public:
	APlayerStatsCharacter();

	virtual void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	void OnInteractTriggered(const FInputActionValue& Value);
	void OnPrimaryActionTriggered(const FInputActionValue& Value);
	void OnSecondaryActionTriggered(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TObjectPtr<UInteractionComponent> InteractionComp;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UHandInventoryComponent> InventoryComp;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SecondaryAction;
};
