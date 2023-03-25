// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwitchChatConnector.h"
#include "RaidEvent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnRaidEventCompleteDelegate);

class ARaid;

UCLASS(Abstract, Blueprintable)
class EPIPHANEBOT_API URaidEvent : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	URaidEvent();

public:

	UFUNCTION(BlueprintNativeEvent)
	bool CanRunEvent();

	UFUNCTION(BlueprintNativeEvent)
	void RunEvent();

	UFUNCTION(BlueprintCallable)
	void AddWinnings(float Amount);

	UFUNCTION(BlueprintCallable)
	bool SendTwitchMessage(FText Message);

	UFUNCTION(BlueprintCallable)
	void MarkComplete();

public:
	UPROPERTY(EditDefaultsOnly)
	int32 Rarity = 0;

	UPROPERTY(BlueprintReadWrite)
	FOnRaidEventCompleteDelegate OnComplete;

	UPROPERTY(BlueprintReadOnly)
	UTwitchChatConnector* Chat;

	UPROPERTY(BlueprintReadOnly)
	ARaid* Raid;
};
