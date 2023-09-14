// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "EpiUserDataSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewUserCreatedDelegate, int32, ID, FString, Name);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FCateriumChangedDelegate, int32, ID, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCateriumChangedBroadcastDelegate, int32, ID, int32, NewValue);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPrestigeChangedDelegate, int32, ID, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPrestigeChangedBroadcastDelegate, int32, ID, int32, NewValue);

USTRUCT(BlueprintType)
struct EPIPHANEBOT_API FEpiUserData
{
	GENERATED_BODY();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat Player", Meta = (ExposeOnSpawn = true))
	int32 ID = 50;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat Player")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Caterium = 15;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 LockedCaterium = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat Player")
	int32 Prestige = 0;
};

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API UEpiUserDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION()
	void OnDatabasePathChanged(FString NewPath);
	void ReloadAllData();
	bool UnlockAllCaterium();
	void EnsureInitialized();

private:
	static bool EnsureUserTable();

	bool GetUserData(const FString& Properties, const uint32 ID, FEpiUserData& OutUserInfo);

public:
	// Existence
	UFUNCTION(BlueprintCallable)
	bool Exists(int32 ID);
	bool Create(int32 ID, const FString& Name);
	bool GetUserData(const uint32 ID, FEpiUserData& OutUserInfo);
	int32 GetIdForName(FString Name);
	
	UFUNCTION(BlueprintPure)
	FString GetNameForId(int32 ID);
	
public:
	// Caterium
	UFUNCTION(BlueprintCallable)
	int32 GetCaterium(int32 ID);

	UFUNCTION(BlueprintCallable)
	void AddCaterium(int32 ID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	void SetCaterium(int32 ID, int32 Amount);

public:
	// Locked Caterium
	UFUNCTION(BlueprintCallable)
	int32 GetLockedCaterium(int32 ID);

	UFUNCTION(BlueprintCallable)
	void LockCaterium(int32 ID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	void UnlockCaterium(int32 ID);

	UFUNCTION(BlueprintCallable)
	void ForefeitLockedCaterium(int32 ID);

public:
	// Prestige
	UFUNCTION(BlueprintCallable)
	int32 GetPrestige(int32 ID);

	UFUNCTION(BlueprintCallable)
	void AddPrestige(int32 ID, int32 Amount);

	UFUNCTION(BlueprintCallable)
	void SetPrestige(int32 ID, int32 Amount);

public:
	// Bindings
	FNewUserCreatedDelegate& GetUserCreatedEvent() { return NewUserCreatedEvent; }

	UFUNCTION(BlueprintCallable)
	void BindOnCateriumChanged(int32 ID, FCateriumChangedDelegate Callback);

	UFUNCTION(BlueprintCallable)
	void BindOnPrestigeChanged(int32 ID, FPrestigeChangedDelegate Callback);

private:

	bool bInitialized = false;

	// Internal Delegate Management
	void NotifyCateriumChanged(int32 ID, int32 NewValue);
	void NotifyPrestigeChanged(int32 ID, int32 NewValue);

	UPROPERTY(BlueprintAssignable)
	FNewUserCreatedDelegate NewUserCreatedEvent;

	UPROPERTY()
	TMap<int32, FCateriumChangedBroadcastDelegate> CateriumChangedDelegates;

	UPROPERTY(BlueprintAssignable)
	FPrestigeChangedBroadcastDelegate PrestigeChangedEvent;

	UPROPERTY()
	TMap<int32, FPrestigeChangedBroadcastDelegate> PrestigeChangedDelegates;
};
