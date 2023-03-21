// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TwitchChatConnector.h"
#include "GameFramework/SaveGame.h"
#include "ChatPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCateriumOverflow, AChatPlayer*, Player);

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API AChatPlayer : public AActor
{
	GENERATED_BODY()

private:
	static bool EnsureTable();

public:
	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (DisplayName = "Get Chat Player", AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static AChatPlayer* GetFromAuthor(UObject* WorldContextObject, FTwitchMessageAuthor Author, TSubclassOf<AChatPlayer> Class);

	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static AChatPlayer* Find(UObject* WorldContextObject, FString Name, TSubclassOf<AChatPlayer> Class);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 ID = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Caterium = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 LockedCaterium = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Prestige = 0;
};
