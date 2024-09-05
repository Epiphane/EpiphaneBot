// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RaidLocation.generated.h"

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API URaidLocation : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FString> AcceptedShorthands;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UTexture2D*> Images;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag LocationType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<FGameplayTag> LocationTags;
};
