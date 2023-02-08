// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Raid.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RaidManager.generated.h"

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API URaidManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsRaidOngoing();

	UFUNCTION(BlueprintCallable)
	ARaid* StartRaid();

	UFUNCTION(BlueprintCallable)
	ARaid* GetCurrentRaid();

private:
	void OnRaidDone(ARaid* Raid);

private:
	UPROPERTY()
	ARaid* CurrentRaid;
};
