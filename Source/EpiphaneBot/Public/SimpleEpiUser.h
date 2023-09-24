// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EpiUser.h"
#include "TwitchChatConnector.h"
#include "SimpleEpiUser.generated.h"

UCLASS(BlueprintType)
class EPIPHANEBOT_API USimpleEpiUser : public UObject, public IEpiUser
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Simple User", WorldContext = "WorldContextObject"))
	static USimpleEpiUser* Get(UObject* WorldContextObject, FString Name, int64 Id, bool CreateIfNotFound = true);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static USimpleEpiUser* GetUserFromName(UObject* WorldContextObject, FString Name);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static USimpleEpiUser* GetUserFromAuthor(UObject* WorldContextObject, FTwitchMessageAuthor Author, bool CreateIfNotFound = true);

	virtual UChatAvatar* GetAvatar_Implementation() const override;
	virtual int32 GetID_Implementation() const override;
	virtual FString GetUserName_Implementation() const override;
	virtual int32 GetCaterium_Implementation() const override;
	virtual int32 GetPrestige_Implementation() const override;
	virtual void AddCaterium_Implementation(int32 Caterium) override;
	virtual void LockCaterium_Implementation(int32 Amount) override;
	virtual void UnlockCaterium_Implementation() override;
	virtual void ForefeitLockedCaterium_Implementation() override;
	virtual void GiveCaterium_Implementation(const TScriptInterface<IEpiUser>& Other, int32 Amount) override;
	virtual void BindOnCateriumChanged_Implementation(const FOnUserCateriumChangedDelegate& Callback) override;
	virtual void BindOnPrestigeChanged_Implementation(const FOnUserPrestigeChangedDelegate& Callback) override;

public:
	UFUNCTION()
	void OnCateriumChanged(int32 ID, int32 NewCaterium);

	UFUNCTION()
	void OnPrestigeChanged(int32 ID, int32 NewPrestige);

protected:
	UPROPERTY(BlueprintAssignable)
	FUserCateriumChangedDelegate OnCateriumChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUserPrestigeChangedDelegate OnPrestigeChangedDelegate;

private:
	UPROPERTY(EditDefaultsOnly)
	UChatAvatar* Avatar;

	UPROPERTY()
	FEpiUserData Data;

	UPROPERTY()
	UEpiUserDataSubsystem* DataSource;
};
