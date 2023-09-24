// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EpiUser.h"
#include "GameFramework/SaveGame.h"
#include "TwitchChatConnector.h"
#include "ChatPlayer.generated.h"

UCLASS(BlueprintType)
class EPIPHANEBOT_API AChatPlayer : public AActor, public IEpiUser
{
	GENERATED_BODY()

public:
	AChatPlayer();

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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UChatAvatar* Avatar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TScriptInterface<IEpiUser> User;
};
