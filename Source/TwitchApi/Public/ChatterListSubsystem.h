// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ChatterListSubsystem.generated.h"

class UTwitchEventsSubsystem;
class UTwitchGetChatters;

/**
 * 
 */
UCLASS()
class TWITCHAPI_API UChatterListSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION()
	void OnConnected(FString Channel);

	UFUNCTION()
	void OnDisconnected();

	UFUNCTION()
	void OnTimerActivate();

	UFUNCTION()
	void OnQuerySuccess(const TArray<FString>& Viewers);

	UFUNCTION()
	void OnQueryFail(const TArray<FString>& Viewers);

private:
	UPROPERTY()
	UTwitchEventsSubsystem* Events;

	UPROPERTY()
	UTwitchGetChatters* QueryTask;

	UPROPERTY()
	FTimerHandle PollTimer;

	UPROPERTY()
	TArray<FString> ChatterIds;
};
