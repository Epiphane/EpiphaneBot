// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "ApplicationSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDatabasePathChangedDelegate, FString, NewPath);

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API UApplicationSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UApplicationSettings(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	static UApplicationSettings* GetApplicationSettings();

public:
	UFUNCTION(BlueprintPure)
	FString GetDatabasePath() const { return DatabasePath; }

	UFUNCTION(BlueprintCallable)
	void SelectNewDatabasePath();

	FOnDatabasePathChangedDelegate& GetOnDatabasePathChanged() { return OnDatabasePathChanged; }

private:
	UPROPERTY()
	FString DatabasePath;

	UPROPERTY(BlueprintAssignable)
	FOnDatabasePathChangedDelegate OnDatabasePathChanged;
};
