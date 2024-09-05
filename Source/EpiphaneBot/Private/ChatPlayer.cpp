// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatPlayer.h"
#include "EpiGameInstance.h"
#include "Kismet/GameplayStatics.h"

AChatPlayer::AChatPlayer()
{
}

AChatPlayer* AChatPlayer::Spawn(UObject* worldContextObject, TSubclassOf<AChatPlayer> Class, TScriptInterface<IEpiUser> Data)
{
	AChatPlayer* Result = worldContextObject->GetWorld()->SpawnActorDeferred<AChatPlayer>(Class, FTransform::Identity);
	check(Result != nullptr);
	Result->User = Data;
	Result->FinishSpawning(FTransform::Identity);
	return Result;
}

int32 AChatPlayer::GetID_Implementation() const
{
	if (!User) return -1;
	return User->Execute_GetID(User.GetObject());
}

FString AChatPlayer::GetUserName_Implementation() const
{
	if (!User) return TEXT("N/A");
	return User->Execute_GetUserName(User.GetObject());
}

FLinearColor AChatPlayer::GetUserColor_Implementation() const
{
	if (!User) return FLinearColor::White;
	return User->Execute_GetUserColor(User.GetObject());
}

int32 AChatPlayer::GetCaterium_Implementation() const
{
	if (!User) return 0;
	return User->Execute_GetCaterium(User.GetObject());
}

int32 AChatPlayer::GetPrestige_Implementation() const
{
	if (!User) return 0;
	return User->Execute_GetPrestige(User.GetObject());
}

void AChatPlayer::AddCaterium_Implementation(int32 Delta)
{
	if (!User) return;
	return User->Execute_AddCaterium(User.GetObject(), Delta);
}

void AChatPlayer::LockCaterium_Implementation(int32 Amount)
{
	if (!User) return;
	return User->Execute_LockCaterium(User.GetObject(), Amount);
}

void AChatPlayer::UnlockCaterium_Implementation()
{
	if (!User) return;
	return User->Execute_UnlockCaterium(User.GetObject());
}

void AChatPlayer::ForefeitLockedCaterium_Implementation()
{
	if (!User) return;
	return User->Execute_ForefeitLockedCaterium(User.GetObject());
}

void AChatPlayer::GiveCaterium_Implementation(const TScriptInterface<IEpiUser>& Other, int32 Amount)
{
	if (!User) return;
	return User->Execute_GiveCaterium(User.GetObject(), Other, Amount);
}

void AChatPlayer::BindOnCateriumChanged_Implementation(const FOnUserCateriumChangedDelegate& Callback)
{
	if (!User) return;
	return User->Execute_BindOnCateriumChanged(User.GetObject(), Callback);
}

void AChatPlayer::BindOnPrestigeChanged_Implementation(const FOnUserPrestigeChangedDelegate& Callback)
{
	if (!User) return;
	return User->Execute_BindOnPrestigeChanged(User.GetObject(), Callback);
}