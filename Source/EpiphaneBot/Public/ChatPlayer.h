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
	UFUNCTION(BlueprintCallable, Category = "Chat Player", meta = (DisplayName = "Get Chat Player"))
	static AChatPlayer* GetFromAuthor(FTwitchMessageAuthor Author);

	UFUNCTION(BlueprintCallable, Category = "Chat Player")
	static AChatPlayer* FindOrCreate(int64 Id, FString Name);

	UFUNCTION(BlueprintCallable, Category = "Chat Player")
	bool RefreshStats();

	UPROPERTY(BlueprintAssignable)
	FOnCateriumOverflow OnCateriumOverflow;

public:
	UFUNCTION(BlueprintCallable)
	void AddCaterium(int32 Caterium);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Caterium;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Prestige;
};
