// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatPlayer.h"
#include "SqliteConnection.h"
#include "sqlite3.h"

bool AChatPlayer::EnsureTable()
{
	auto SqliteConnection = USqliteConnection::OpenDefault();
	if (!SqliteConnection.IsValid())
	{
		return false;
	}

	return SqliteConnection->Execute(R"(CREATE TABLE IF NOT EXISTS "User" (
		"Id"				INTEGER,
		"Name"				TEXT UNIQUE NOT NULL,
		"Caterium"			INTEGER DEFAULT 15,
		"LockedCaterium"	INTEGER DEFAULT 0,
		"Prestige"			INTEGER NOT NULL DEFAULT 0,
		PRIMARY KEY("Id")
	))");
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

AChatPlayer* AChatPlayer::Find(UObject* WorldContextObject, FString Name, TSubclassOf<AChatPlayer> Class)
{
	auto Select = USqliteConnection::PrepareSimple(TEXT(R"(SELECT Id FROM "User" WHERE Name = ?)"));
	if (!Select.IsValid() ||
		!Select.Bind(1, Name) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return nullptr;
	}

	TMap<FString, FSQLiteValue> Properties = Select.ReadRow();

	AChatPlayer* ChatPlayer = WorldContextObject->GetWorld()->SpawnActor<AChatPlayer>(Class);
	ChatPlayer->ID = Properties["Id"].IntValue;
	ChatPlayer->Name = Name;
	if (ChatPlayer->ReloadData())
	{
		return ChatPlayer;
	}

	return nullptr;
}

AChatPlayer* AChatPlayer::FindOrCreate(UObject* WorldContextObject, int64 ID, FString Name, TSubclassOf<AChatPlayer> Class)
{
	AChatPlayer* ChatPlayer = WorldContextObject->GetWorld()->SpawnActor<AChatPlayer>(Class);
	ChatPlayer->ID = ID;
	ChatPlayer->Name = Name;
	if (ChatPlayer->ReloadData())
	{
		return ChatPlayer;
	}

	// Ensure player exists
	auto Insert = USqliteConnection::PrepareSimple(TEXT(R"(INSERT INTO "User" (Id, Name) VALUES (?, ?))"));
	if (!Insert.IsValid() ||
		!Insert.Bind(1, ID) ||
		!Insert.Bind(2, Name) ||
		Insert.Step() != ESqliteStepResult::Done)
	{
		return nullptr;
	}

	ChatPlayer->ReloadData();
	return ChatPlayer;
}

bool AChatPlayer::ReloadData()
{
	if (!EnsureTable())
	{
		return false;
	}

	auto Select = USqliteConnection::PrepareSimple(TEXT(R"(SELECT Id, Name, Caterium, Prestige FROM "User" WHERE Id = ?)"));
	if (!Select.IsValid() ||
		!Select.Bind(1, ID) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return false;
	}

	Select.AssignNextRowToObject(this);
	return true;
}

void AChatPlayer::AddCaterium(int32 Delta)
{
	int32 NewCaterium = Caterium + Delta;
	if (Delta > 0)
	{
		// Overflow => Prestige :D
		if (NewCaterium < 0)
		{
			OnCateriumOverflow.Broadcast(this);
		}
	}
	else
	{
		NewCaterium = FMath::Max(NewCaterium, 0);
	}

	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Caterium = ? WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, NewCaterium) ||
		!Update.Bind(2, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		return;
	}
}

void AChatPlayer::LockCaterium(int32 Amount)
{
	if (Amount == 0)
	{
		return;
	}

	Amount = FMath::Min(Amount, Caterium);

	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Caterium = ?, LockedCaterium = ? WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, Caterium - Amount) ||
		!Update.Bind(2, LockedCaterium + Amount) ||
		!Update.Bind(3, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		return;
	}

	Caterium -= Amount;
	LockedCaterium += Amount;
}

void AChatPlayer::UnlockCaterium()
{
	LockCaterium(-LockedCaterium);
}

void AChatPlayer::ForefeitLockedCaterium()
{
	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET LockedCaterium = 0 WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		return;
	}

	LockedCaterium = 0;
}

void AChatPlayer::GiveCaterium(AChatPlayer* Other, int32 Amount)
{
	if (!ensure(Other))
	{
		return;
	}

	if (Caterium < Amount || Amount < 0)
	{
		return;
	}

	AddCaterium(-Amount);
	Other->AddCaterium(Amount);
}
