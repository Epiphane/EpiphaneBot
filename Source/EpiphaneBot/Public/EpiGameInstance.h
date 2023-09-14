// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EpiGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewPlayerJoinedDelegate, FString, UserName, int64, UserId);

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API UEpiGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static UEpiGameInstance* GetEpiGameInstance(UObject * WorldContextObject) { return Cast<UEpiGameInstance>(WorldContextObject->GetWorld()->GetGameInstance()); }

public:
	UFUNCTION()
	void OnNewUserCreated(int32 ID, FString Name);

	UFUNCTION()
	FOnNewPlayerJoinedDelegate& GetOnNewPlayerJoined() { return OnNewPlayerJoined; }
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintAssignable)
	FOnNewPlayerJoinedDelegate OnNewPlayerJoined;
};
