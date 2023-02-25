// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Raid.h"
#include "RaidEvent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RaidManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRaidManager, Log, All);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaidCooldownOverDelegate);

UENUM(BlueprintType)
enum class ERaidEligibility :uint8
{
	NoCurrentRaid,
	RaidIsJoinable,
	AlreadyParticipating,
	RaidIsOngoing,
	Cooldown,
};

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API URaidManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	bool IsRaidOngoing();

	UFUNCTION(BlueprintCallable)
	ARaid* StartRaid(TSubclassOf<ARaid> RaidClass);

	UFUNCTION(BlueprintCallable)
	ARaid* GetCurrentRaid();

	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "Result"))
	void GetRaidEligibility(AChatPlayer* Player, ERaidEligibility& Result, ARaid*& CurrentRaid, URaidParticipantComponent*& Participant, float& Cooldown);

	FOnRaidCooldownOverDelegate& GetOnRaidCooldownOver() { return OnRaidCooldownOver; }

	UPROPERTY(BlueprintReadWrite)
	UTwitchChatConnector* Chat;

	UPROPERTY(BlueprintReadWrite)
	FText RaidPossibleMessage;

private:
	UFUNCTION()
	void OnRaidDone(ARaid* Raid);

	UFUNCTION()
	void OnCooldownOver();

private:
	UPROPERTY()
	ARaid* CurrentRaid;

	UPROPERTY(BlueprintAssignable)
	FOnRaidCooldownOverDelegate OnRaidCooldownOver;

	/* Handle to manage the timer */
	FTimerHandle RaidCooldownTimer;

	int64 RaidEventPoolSize = 0;

	TArray<TSubclassOf<URaidEvent>> AvailableEvents;
};
