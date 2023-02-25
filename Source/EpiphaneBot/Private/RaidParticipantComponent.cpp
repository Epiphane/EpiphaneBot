// Fill out your copyright notice in the Description page of Project Settings.

#include "RaidParticipantComponent.h"
#include "Raid.h"
#include "SqliteConnection.h"

bool URaidParticipantComponent::EnsureTable()
{
	if (!ARaid::EnsureTable())
	{
		return false;
	}

	auto SqliteConnection = USqliteConnection::OpenDefault();
	if (!SqliteConnection.IsValid())
	{
		return false;
	}

	return SqliteConnection->Execute(R"(CREATE TABLE IF NOT EXISTS "RaidParticipant" (
		"RaidId"		INTEGER NOT NULL,
		"UserId"		INTEGER NOT NULL,
		"Investment"	INTEGER NOT NULL,
		"Health"		INTEGER NOT NULL DEFAULT 100,
		FOREIGN KEY		("UserId") REFERENCES "User"("Id"),
		FOREIGN KEY		("RaidId") REFERENCES "Raid"("Id")
	))");
}

void URaidParticipantComponent::BeginPlay()
{
	if (!ensureAlways(Raid))
	{
		return;
	}

	AChatPlayer* Player = GetOwner<AChatPlayer>();
	check(Player);

	if (!EnsureTable())
	{
		return;
	}

	auto Insert = USqliteConnection::PrepareSimple(TEXT(R"(INSERT INTO "RaidParticipant" (RaidId, UserId, Investment) VALUES (?, ?, ?))"));
	if (!Insert.IsValid() ||
		!Insert.Bind(1, Raid->ID) ||
		!Insert.Bind(2, Player->ID) ||
		!Insert.Bind(3, Investment) ||
		Insert.Step() != ESqliteStepResult::Done)
	{
		return;
	}
}

void URaidParticipantComponent::SetInvestment(int32 NewInvestment)
{
	if (!ensureAlways(Raid))
	{
		return;
	}

	AChatPlayer* Player = GetOwner<AChatPlayer>();
	check(Player);

	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "RaidParticipant" SET Investment = ? WHERE RaidId = ? AND UserId = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, NewInvestment) ||
		!Update.Bind(2, Raid->ID) ||
		!Update.Bind(3, Player->ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		return;
	}

	Investment = NewInvestment;
}

void URaidParticipantComponent::SetHealth(int32 NewHealth)
{
	if (!ensureAlways(Raid))
	{
		return;
	}

	AChatPlayer* Player = GetOwner<AChatPlayer>();
	check(Player);

	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "RaidParticipant" SET Health = ? WHERE RaidId = ? AND UserId = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, NewHealth) ||
		!Update.Bind(2, Raid->ID) ||
		!Update.Bind(3, Player->ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		return;
	}

	Health = NewHealth;
}
