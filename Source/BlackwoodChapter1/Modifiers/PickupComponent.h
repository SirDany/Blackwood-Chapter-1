
#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Modifiers/InteractableComponent.h"
#include "PickupComponent.generated.h"

class AActor;
class UPrimitiveComponent;

USTRUCT()
struct FPickupPrimitiveState
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> Component = nullptr;

	ECollisionEnabled::Type CollisionEnabled =
		ECollisionEnabled::QueryAndPhysics;

	bool bSimulatePhysics = false;
};

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UPickupComponent
	: public UInteractableComponent
{
	GENERATED_BODY()

public:
	UPickupComponent();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetHeldState(bool bHeld);

	UFUNCTION(BlueprintPure, Category = "Pickup|Held")
	FTransform GetHeldRelativeTransform() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText DisplayName = FText::FromString("Item");

	// Position and rotation relative to the camera when held.
	// Scale is calculated separately.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Held")
	FTransform HeldRelativeTransform = FTransform(
		FRotator::ZeroRotator,
		FVector(25.0f, 10.0f, -10.0f),
		FVector::OneVector
	);

	// Multiplier applied to the item's original world scale when held.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Held",
		meta = (ClampMin = "0.01", UIMin = "0.01"))
	float HeldScaleMultiplier = 0.25f;

	// If enabled, calculate held scale using the actor's bounding dimensions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Held")
	bool bUseAutomaticHeldScale = false;

	// Target longest dimension while held, in centimeters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup|Held",
		meta = (ClampMin = "1.0", UIMin = "1.0",
			EditCondition = "bUseAutomaticHeldScale"))
	float TargetHeldSize = 25.0f;

protected:
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual FText GetInteractionPrompt_Implementation() const override;

private:
	bool TryPickup(AActor* Interactor);

	UPROPERTY(Transient)
	TArray<FPickupPrimitiveState> SavedPrimitiveStates;

	FVector OriginalWorldScale = FVector::OneVector;

	bool bPreviousActorCollisionEnabled = true;
	bool bHasSavedState = false;
};
