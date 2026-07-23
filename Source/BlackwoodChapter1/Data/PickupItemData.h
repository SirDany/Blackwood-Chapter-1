// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupItemData.generated.h"

/**
 * 
 */
UCLASS()
class BLACKWOODCHAPTER1_API UPickupItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    UStaticMesh* WorldMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    bool bStackable = false;

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    int MaxStack = 100;
};
