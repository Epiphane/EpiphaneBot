// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChatterListSubsystem.h"
#include "EpiGameChatterSubsystem.generated.h"

/**
 * Specialized version of ChatterListSubsystem that only returns people who have played the chat minigame.
 */
UCLASS()
class EPIPHANEBOT_API UEpiGameChatterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
