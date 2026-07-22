#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class BLACKWOODCHAPTER1_API IInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    void Interact(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    FText GetInteractionPrompt() const;
};