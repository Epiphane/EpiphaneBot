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
		"Id"		INTEGER,
		"Name"		TEXT UNIQUE,
		"Caterium"	INTEGER DEFAULT 0,
		"Prestige"	INTEGER NOT NULL DEFAULT 0,
		PRIMARY KEY("Id")
	))");
}

AChatPlayer* AChatPlayer::GetFromAuthor(FTwitchMessageAuthor Author)
{
	FString name = Author.Name;
	int64 id = FCString::Strtoi64(*Author.UserId, nullptr, 10);
	if (id == 0)
	{
		return nullptr;
	}

	return FindOrCreate(id, name);
}

AChatPlayer* AChatPlayer::FindOrCreate(int64 Id, FString Name)
{
	if (!EnsureTable())
	{
		return nullptr;
	}

	auto SqliteConnection = USqliteConnection::OpenDefault();
	if (!SqliteConnection.IsValid())
	{
		return nullptr;
	}

	{
		auto Insert = SqliteConnection->Prepare(TEXT(R"(INSERT OR IGNORE INTO "User" (Id, Name) VALUES (?, ?))"));
		if (!Insert.IsValid())
		{
			return nullptr;
		}

		if (!Insert.Bind(1, Id) ||
			!Insert.Bind(2, Name))
		{
			return nullptr;
		}

		if (Insert.Step() != ESqliteStepResult::Done)
		{
			return nullptr;
		}
	}

	{
		auto Select = SqliteConnection->Prepare(TEXT(R"(SELECT Id, Name, Caterium, Prestige FROM "User" WHERE Id = ?)"));
		if (!Select.IsValid())
		{
			return nullptr;
		}

		if (!Select.Bind(1, Id))
		{
			return nullptr;
		}

		if (Select.Step() != ESqliteStepResult::Data)
		{
			return nullptr;
		}

		AChatPlayer* ChatPlayer = NewObject<AChatPlayer>();
		Select.AssignNextRowToObject(ChatPlayer);
		return ChatPlayer;
	}

    return nullptr;
}

bool AChatPlayer::RefreshStats()
{
	return false;
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
}
