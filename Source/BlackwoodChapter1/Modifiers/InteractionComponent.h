#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UInteractionPromptWidget;

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class BLACKWOODCHAPTER1_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "Interaction|UI")
	TSubclassOf<UInteractionPromptWidget> InteractionPromptWidgetClass;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	FVector GetOwnerViewLocation() const;
	FRotator GetOwnerViewRotation() const;

private:
	void UpdateInteractionPrompt();

	UPROPERTY(Transient)
	TObjectPtr<UInteractionPromptWidget> InteractionPromptWidget;
};