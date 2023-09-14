// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EpiUser.h"
#include "GameFramework/SaveGame.h"
#include "TwitchChatConnector.h"
#include "ChatPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCateriumOverflow, AChatPlayer*, Player);

/**
 * 
 */
UCLASS(BlueprintType)
class EPIPHANEBOT_API AChatPlayer : public AEpiUser
{
	GENERATED_BODY()

public:
	AChatPlayer();

	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (DisplayName = "Get Chat Player", AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static AChatPlayer* GetFromAuthor(UObject* WorldContextObject, FTwitchMessageAuthor Author, TSubclassOf<AChatPlayer> Class);

	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (WorldContext = "WorldContextObject"))
	static bool Exists(UObject* WorldContextObject, int64 Id);

	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static AChatPlayer* Find(UObject* WorldContextObject, FString Name, TSubclassOf<AChatPlayer> Class);

	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static AChatPlayer* FindById(UObject* WorldContextObject, int64 Id, TSubclassOf<AChatPlayer> Class);

	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static AChatPlayer* FindOrCreate(UObject* WorldContextObject, int64 Id, FString Name, TSubclassOf<AChatPlayer> Class);

	UFUNCTION(BlueprintCallable, Category = "Chat Player")
	bool ReloadData();

	UPROPERTY(BlueprintAssignable)
	FOnCateriumOverflow OnCateriumOverflow;

public:
	UFUNCTION(BlueprintCallable)
	void AddCaterium(int32 Caterium);

	UFUNCTION(BlueprintCallable)
	void LockCaterium(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void UnlockCaterium();

	UFUNCTION(BlueprintCallable)
	void ForefeitLockedCaterium();

	UFUNCTION(BlueprintCallable)
	void GiveCaterium(AChatPlayer* Other, int32 Amount);

private:
	UPROPERTY()
	UEpiUserDataSubsystem* UserData;
};
