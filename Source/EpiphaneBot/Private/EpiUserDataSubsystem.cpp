// Fill out your copyright notice in the Description page of Project Settings.


#include "EpiUserDataSubsystem.h"
#include "SqliteConnection.h"
#include "ApplicationSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogEpiUserDataSubsystem, Log, All);

void UEpiUserDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	EnsureInitialized();
	ReloadAllData();
}

void UEpiUserDataSubsystem::EnsureInitialized()
{
	if (!bInitialized && GEngine)
	{
		UApplicationSettings* UserSettings = Cast<UApplicationSettings>(GEngine->GetGameUserSettings());
		if (ensure(UserSettings))
		{
			UserSettings->GetOnDatabasePathChanged().AddDynamic(this, &UEpiUserDataSubsystem::OnDatabasePathChanged);
			bInitialized = true;
		}
	}
}

void UEpiUserDataSubsystem::OnDatabasePathChanged(FString NewPath)
{
	ReloadAllData();
}

void UEpiUserDataSubsystem::ReloadAllData()
{
	EnsureInitialized();
	
	if (!UnlockAllCaterium())
	{
		UE_LOG(LogEpiUserDataSubsystem, Error, TEXT("Failed unlocking all caterium"));
	}

	TSet<int32> UserIDs;
	for (typename auto It(CateriumChangedDelegates.CreateConstIterator()); It; ++It)
	{
		UserIDs.Emplace(It->Key);
	}

	for (typename auto It(PrestigeChangedDelegates.CreateConstIterator()); It; ++It)
	{
		UserIDs.Emplace(It->Key);
	}

	FEpiUserData Data;
	for (const int32 ID : UserIDs)
	{
		GetUserData(ID, Data);

		if (auto* Notifier = CateriumChangedDelegates.Find(ID))
		{
			Notifier->Broadcast(ID, Data.Caterium);
		}

		if (auto* Notifier = PrestigeChangedDelegates.Find(ID))
		{
			Notifier->Broadcast(ID, Data.Prestige);
		}
	}
}

bool UEpiUserDataSubsystem::UnlockAllCaterium()
{
	auto SqliteConnection = USqliteConnection::OpenDefault();
	if (!SqliteConnection.IsValid())
	{
		return false;
	}

	return SqliteConnection->Execute(R"(UPDATE "User" SET Caterium = Caterium + LockedCaterium, LockedCaterium = 0 WHERE 1)");
}

bool UEpiUserDataSubsystem::EnsureUserTable()
{
	auto SqliteConnection = USqliteConnection::OpenDefault();
	if (!SqliteConnection.IsValid())
	{
		return false;
	}

	return SqliteConnection->Execute(R"X(CREATE TABLE IF NOT EXISTS "User" (
		"Id"				INTEGER,
		"Name"				TEXT UNIQUE NOT NULL,
		"Caterium"			INTEGER DEFAULT 15,
		"LockedCaterium"	INTEGER DEFAULT 0,
		"Prestige"			INTEGER NOT NULL DEFAULT 0,
		"Color"				TEXT NOT NULL DEFAULT "(R=255,G=255,B=255,A=255)",
		PRIMARY KEY("Id")
	))X");
}

bool UEpiUserDataSubsystem::GetUserData(const FString& Properties, const uint32 ID, FEpiUserData& OutUserInfo)
{
	if (!EnsureUserTable())
	{
		return false;
	}

	auto Select = USqliteConnection::PrepareSimple(FString::Printf(TEXT(R"(SELECT %s FROM "User" WHERE Id = ?)"), *Properties));
	if (!Select.IsValid() ||
		!Select.Bind(1, ID) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return false;
	}

	Select.AssignNextRowToObject(&OutUserInfo);
	return true;
}

bool UEpiUserDataSubsystem::Exists(int32 ID)
{
	auto Select = USqliteConnection::PrepareSimple(TEXT(R"(SELECT Id FROM "User" WHERE Id = ?)"));
	if (!Select.IsValid() ||
		!Select.Bind(1, ID) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return false;
	}

	return true;
}

bool UEpiUserDataSubsystem::Create(int32 ID, const FString& Name)
{
	auto Insert = USqliteConnection::PrepareSimple(TEXT(R"(INSERT INTO "User" (Id, Name) VALUES (?, ?))"));
	if (!Insert.IsValid() ||
		!Insert.Bind(1, ID) ||
		!Insert.Bind(2, Name) ||
		Insert.Step() != ESqliteStepResult::Done)
	{
		return false;
	}

	NewUserCreatedEvent.Broadcast(ID, Name);
	return true;
}

bool UEpiUserDataSubsystem::GetUserData(const uint32 ID, FEpiUserData& OutUserInfo)
{
	return GetUserData(TEXT("Id, Name, Color, Caterium, LockedCaterium, Prestige"), ID, OutUserInfo);
}

int32 UEpiUserDataSubsystem::GetIdForName(FString Name)
{
	auto Select = USqliteConnection::PrepareSimple(TEXT(R"(SELECT Id FROM "User" WHERE Name = ? COLLATE NOCASE)"));
	if (!Select.IsValid() ||
		!Select.Bind(1, Name) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return -1;
	}

	TMap<FString, FSQLiteValue> Properties = Select.ReadRow();
	return Properties["Id"].IntValue;
}

FString UEpiUserDataSubsystem::GetNameForId(int32 ID)
{
	auto Select = USqliteConnection::PrepareSimple(TEXT(R"(SELECT Name FROM "User" WHERE Id = ?)"));
	if (!Select.IsValid() ||
		!Select.Bind(1, ID) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return "";
	}

	TMap<FString, FSQLiteValue> Properties = Select.ReadRow();
	return Properties["Name"].StringValue;
}

FLinearColor UEpiUserDataSubsystem::GetColor(int32 ID)
{
	FEpiUserData User;
	if (!GetUserData(TEXT("Color"), ID, User))
	{
		return FLinearColor::White;
	}

	return User.Color;
}

void UEpiUserDataSubsystem::SetColor(int32 ID, FLinearColor Color)
{
	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Color = ? WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, Color.ToString()) ||
		!Update.Bind(2, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		UE_LOG(LogEpiUserDataSubsystem, Error, TEXT("Failed setting color for user %d to %s"), ID, *Color.ToString());
		return;
	}

	NotifyColorChanged(ID, Color);
}

int32 UEpiUserDataSubsystem::GetCaterium(int32 ID)
{
	FEpiUserData User;
	if (!GetUserData(TEXT("Caterium"), ID, User))
	{
		return -1;
	}

	return User.Caterium;
}

void UEpiUserDataSubsystem::AddCaterium(int32 ID, int32 Amount)
{
	int32 Caterium = GetCaterium(ID);
	if (Caterium < 0)
	{
		return;
	}

	int32 NewCaterium = Caterium + Amount;
	if (Amount > 0)
	{
		// Overflow => Prestige :D
		if (NewCaterium < 0)
		{
			AddPrestige(ID, 1);
			NewCaterium -= TNumericLimits<int32>::Max();
		}
	}
	else
	{
		NewCaterium = FMath::Max(NewCaterium, 0);
	}

	SetCaterium(ID, NewCaterium);
}

void UEpiUserDataSubsystem::SetCaterium(int32 ID, int32 Amount)
{
	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Caterium = ? WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, Amount) ||
		!Update.Bind(2, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		UE_LOG(LogEpiUserDataSubsystem, Error, TEXT("Failed setting caterium for user %d to %d"), ID, Amount);
		return;
	}

	NotifyCateriumChanged(ID, Amount);
}

int32 UEpiUserDataSubsystem::GetLockedCaterium(int32 ID)
{
	FEpiUserData User;
	if (!GetUserData(TEXT("LockedCaterium"), ID, User))
	{
		return -1;
	}

	return User.LockedCaterium;
}

void UEpiUserDataSubsystem::LockCaterium(int32 ID, int32 Amount)
{
	if (Amount == 0)
	{
		return;
	}

	FEpiUserData User;
	if (!GetUserData(TEXT("Caterium, LockedCaterium"), ID, User))
	{
		return;
	}

	Amount = FMath::Min(Amount, User.Caterium);

	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Caterium = ?, LockedCaterium = ? WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, User.Caterium - Amount) ||
		!Update.Bind(2, User.LockedCaterium + Amount) ||
		!Update.Bind(3, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		UE_LOG(LogEpiUserDataSubsystem, Error, TEXT("Failed locking caterium for user %d"), ID);
		return;
	}

	NotifyCateriumChanged(ID, User.Caterium - Amount);
}

void UEpiUserDataSubsystem::UnlockCaterium(int32 ID)
{
	FEpiUserData User;
	if (!GetUserData(TEXT("Caterium, LockedCaterium"), ID, User))
	{
		return;
	}

	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Caterium = ?, LockedCaterium = 0 WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, User.Caterium + User.LockedCaterium) ||
		!Update.Bind(2, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		UE_LOG(LogEpiUserDataSubsystem, Error, TEXT("Failed unlocking caterium for user %d"), ID);
		return;
	}

	NotifyCateriumChanged(ID, User.Caterium + User.LockedCaterium);
}

void UEpiUserDataSubsystem::ForefeitLockedCaterium(int32 ID)
{
	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET LockedCaterium = 0 WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		return;
	}
}

int32 UEpiUserDataSubsystem::GetPrestige(int32 ID)
{
	FEpiUserData User;
	if (!GetUserData(TEXT("Prestige"), ID, User))
	{
		return -1;
	}

	return User.Prestige;
}

void UEpiUserDataSubsystem::AddPrestige(int32 ID, int32 Amount)
{
	int32 Prestige = GetPrestige(ID);
	if (Prestige < 0)
	{
		return;
	}

	SetPrestige(ID, Prestige + Amount);
}

void UEpiUserDataSubsystem::SetPrestige(int32 ID, int32 Amount)
{
	auto Update = USqliteConnection::PrepareSimple(TEXT(R"(UPDATE "User" SET Prestige = ? WHERE Id = ?)"));
	if (!Update.IsValid() ||
		!Update.Bind(1, Amount) ||
		!Update.Bind(2, ID) ||
		Update.Step() != ESqliteStepResult::Done)
	{
		UE_LOG(LogEpiUserDataSubsystem, Error, TEXT("Failed setting prestige for user %d to %d"), ID, Amount);
		return;
	}

	NotifyPrestigeChanged(ID, Amount);
}

void UEpiUserDataSubsystem::BindOnColorChanged(int32 ID, FColorChangedDelegate Callback)
{
	ColorChangedDelegates.FindOrAdd(ID).Add(Callback);
}

void UEpiUserDataSubsystem::BindOnCateriumChanged(int32 ID, FCateriumChangedDelegate Callback)
{
	CateriumChangedDelegates.FindOrAdd(ID).Add(Callback);
}

void UEpiUserDataSubsystem::BindOnPrestigeChanged(int32 ID, FPrestigeChangedDelegate Callback)
{
	PrestigeChangedDelegates.FindOrAdd(ID).Add(Callback);
}

void UEpiUserDataSubsystem::NotifyColorChanged(int32 ID, FLinearColor NewValue)
{
	if (FColorChangedBroadcastDelegate* Broadcaster = ColorChangedDelegates.Find(ID))
	{
		Broadcaster->Broadcast(ID, NewValue);
	}
}

void UEpiUserDataSubsystem::NotifyCateriumChanged(int32 ID, int32 NewValue)
{
    if (FCateriumChangedBroadcastDelegate* Broadcaster = CateriumChangedDelegates.Find(ID))
    {
        Broadcaster->Broadcast(ID, NewValue);
    }
}

void UEpiUserDataSubsystem::NotifyPrestigeChanged(int32 ID, int32 NewValue)
{
	PrestigeChangedEvent.Broadcast(ID, NewValue);
	if (FPrestigeChangedBroadcastDelegate* Broadcaster = PrestigeChangedDelegates.Find(ID))
	{
		Broadcaster->Broadcast(ID, NewValue);
	}
}
