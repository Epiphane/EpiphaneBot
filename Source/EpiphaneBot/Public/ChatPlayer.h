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
	static AChatPlayer* FindOrCreate(UObject* WorldContextObject, int64 Id, FString Name, TSubclassOf<AChatPlayer> Class);

	UFUNCTION(BlueprintCallable, Category = "Chat Player")
	bool ReloadData();

	UPROPERTY(BlueprintAssignable)
	FOnCateriumOverflow OnCateriumOverflow;

public:
	UFUNCTION(BlueprintCallable)
	void AddCaterium(int32 Caterium);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 ID = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Caterium = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Prestige = 0;
};
