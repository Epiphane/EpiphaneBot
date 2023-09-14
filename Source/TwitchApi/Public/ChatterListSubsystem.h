// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TwitchGetChatters.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ChatterListSubsystem.generated.h"

class UTwitchEventsSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatterDelegate, FChatter, User);

/**
 * 
 */
UCLASS()
class TWITCHAPI_API UChatterListSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION()
	void OnConnected(FString Channel);

	UFUNCTION()
	void OnDisconnected();

	UFUNCTION()
	void OnTimerActivate();

	UFUNCTION()
	void OnQuerySuccess(const TArray<FChatter>& Viewers);

	UFUNCTION()
	void OnQueryFail(const TArray<FChatter>& Viewers);

	virtual void ProcessChanges(const TSet<FChatter>& NewChatters, const TSet<FChatter>& LeftChatters);

	FOnChatterDelegate& GetOnChatterJoined() { return OnChatterJoined; }
	FOnChatterDelegate& GetOnChatterLeft() { return OnChatterLeft; }

protected:
	UPROPERTY(BlueprintAssignable)
	FOnChatterDelegate OnChatterJoined;

	UPROPERTY(BlueprintAssignable)
	FOnChatterDelegate OnChatterLeft;

private:
	UPROPERTY()
	UTwitchEventsSubsystem* Events;

	UPROPERTY()
	UTwitchGetChatters* QueryTask;

	UPROPERTY()
	FTimerHandle PollTimer;

	UPROPERTY()
	TSet<FChatter> ActiveChatters;
};
