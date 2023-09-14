// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatPlayer.h"
#include "EpiGameInstance.h"
#include "Kismet/GameplayStatics.h"

AChatPlayer::AChatPlayer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameInstance* GameInstance = World->GetGameInstance();
		if (GameInstance)
		{
			UserData = GameInstance->GetSubsystem<UEpiUserDataSubsystem>();
			ensure(IsValid(UserData));
		}
	}
}

AChatPlayer* AChatPlayer::GetFromAuthor(UObject* WorldContextObject, FTwitchMessageAuthor Author, TSubclassOf<AChatPlayer> Class)
{
	FString name = Author.Name;
	int64 id = FCString::Strtoi64(*Author.UserId, nullptr, 10);
	if (id == 0)
	{
		return nullptr;
	}

	return FindOrCreate(WorldContextObject, id, name, Class);
}

bool AChatPlayer::Exists(UObject* WorldContextObject, int64 ID)
{
	UEpiUserDataSubsystem* UserData = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (!ensure(IsValid(UserData)))
	{
		return false;
	}

	return UserData->Exists(ID);
}

AChatPlayer* AChatPlayer::Find(UObject* WorldContextObject, FString Name, TSubclassOf<AChatPlayer> Class)
{
	UEpiUserDataSubsystem* UserData = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (!ensure(IsValid(UserData)))
	{
		return false;
	}

	Name.RemoveFromStart(TEXT("@"));

	int id = UserData->GetIdForName(Name);
	if (id < 0)
	{
		return nullptr;
	}

	return FindById(WorldContextObject, id, Class);
}

AChatPlayer* AChatPlayer::FindById(UObject* WorldContextObject, int64 Id, TSubclassOf<AChatPlayer> Class)
{
	if (!Exists(WorldContextObject, Id))
	{
		return nullptr;
	}

	AChatPlayer* ChatPlayer = WorldContextObject->GetWorld()->SpawnActorDeferred<AChatPlayer>(Class, FTransform::Identity);
	ChatPlayer->Data.ID = Id;
	if (ChatPlayer->ReloadData())
	{
		ChatPlayer->FinishSpawning(FTransform::Identity);
		return ChatPlayer;
	}

	ChatPlayer->Destroy();
	return nullptr;
}

AChatPlayer* AChatPlayer::FindOrCreate(UObject* WorldContextObject, int64 ID, FString Name, TSubclassOf<AChatPlayer> Class)
{
	if (!ensure(Class))
	{
		return nullptr;
	}

	AChatPlayer* ChatPlayer = WorldContextObject->GetWorld()->SpawnActorDeferred<AChatPlayer>(Class, FTransform::Identity);
	ChatPlayer->Data.ID = ID;
	ChatPlayer->Data.Name = Name;
	if (ChatPlayer->ReloadData())
	{
		ChatPlayer->FinishSpawning(FTransform::Identity);
		return ChatPlayer;
	}

	if (!ChatPlayer->UserData->Create(ID, Name))
	{
		ChatPlayer->Destroy();
		return nullptr;
	}

	ChatPlayer->ReloadData();
	return ChatPlayer;
}

bool AChatPlayer::ReloadData()
{
	return UserData->GetUserData(Data.ID, Data);
}

void AChatPlayer::AddCaterium(int32 Delta)
{
	return UserData->AddCaterium(Data.ID, Delta);
}

void AChatPlayer::LockCaterium(int32 Amount)
{
	return UserData->LockCaterium(Data.ID, Amount);
}

void AChatPlayer::UnlockCaterium()
{
	return UserData->UnlockCaterium(Data.ID);
}

void AChatPlayer::ForefeitLockedCaterium()
{
	return UserData->ForefeitLockedCaterium(Data.ID);
}

void AChatPlayer::GiveCaterium(AChatPlayer* Other, int32 Amount)
{
	if (!ensure(Other))
	{
		return;
	}

	if (Data.Caterium < Amount || Amount < 0)
	{
		return;
	}

	AddCaterium(-Amount);
	Other->AddCaterium(Amount);
}
