// Fill out your copyright notice in the Description page of Project Settings.

#include "Raid.h"
#include "TwitchPluginBPLibrary.h"
#include "SqliteConnection.h"

DEFINE_LOG_CATEGORY(LogRaid);

bool ARaid::EnsureTable()
{
	auto SqliteConnection = USqliteConnection::OpenDefault();
	if (!SqliteConnection.IsValid())
	{
		return false;
	}

	return SqliteConnection->Execute(R"(CREATE TABLE IF NOT EXISTS "Raid" (
		"Id"			INTEGER,
		"Time"			TEXT NOT NULL,
		"Investment"	INTEGER NOT NULL DEFAULT 0,
		"Return"		INTEGER NOT NULL DEFAULT 0,
		PRIMARY KEY		("Id" AUTOINCREMENT)
	))");
}

// Sets default values
ARaid::ARaid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	State = ERaidState::NotStarted;

}

ARaid* ARaid::CreateRaid(UWorld* worldContext, TSubclassOf<ARaid> RaidClass, TArray<TSubclassOf<URaidEvent>> AvailableEvents, UTwitchChatConnector* Chat)
{
	if (!ensure(RaidClass))
	{
		return nullptr;
	}

	if (!EnsureTable())
	{
		return nullptr;
	}

	FSQLiteValue* Id;
	{
		auto Insert = USqliteConnection::PrepareSimple(TEXT(R"(INSERT INTO "Raid" (Time) VALUES (datetime('now')) RETURNING Id)"));
		if (!Insert.IsValid() ||
			Insert.Step() != ESqliteStepResult::Data)
		{
			return nullptr;
		}

		TMap<FString, FSQLiteValue> Properties = Insert.ReadRow();
		Id = Properties.Find("Id");
		if (Id == nullptr)
		{
			return nullptr;
		}

		if (Id->Type != ESqliteValueType::Integer)
		{
			return nullptr;
		}
	}

	ARaid* RaidObject = worldContext->SpawnActor<ARaid>(RaidClass);
	check(RaidObject != nullptr);
	RaidObject->ID = Id->IntValue;
	RaidObject->Chat = Chat;
	if (!RaidObject->ReloadData())
	{
		RaidObject->Destroy();
		return nullptr;
	}

	for (const auto& EventClass : AvailableEvents)
	{
		URaidEvent* Event = NewObject<URaidEvent>(RaidObject, EventClass);
		Event->Chat = Chat;
		Event->Raid = RaidObject;
		Event->OnComplete.BindDynamic(RaidObject, &ARaid::OnRaidEventComplete);
		RaidObject->AvailableEvents.Add(Event);
	}

	return RaidObject;
}

// Called when the game starts or when spawned
void ARaid::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARaid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARaid::BeginPreparing_Implementation()
{
	State = ERaidState::Preparing;
}

void ARaid::BeginRaid_Implementation()
{
	State = ERaidState::Running;
	Investment = 0;
	MaxInvestment = 0;
	for (const auto& participant : Participants)
	{
		int64 investment = participant->GetInvestment();
		Investment += investment;
		MaxInvestment = FMath::Max(investment, MaxInvestment);
	}
	AverageInvestment = Investment / Participants.Num();

	auto Insert = USqliteConnection::PrepareSimple(TEXT(R"(Update "Raid" SET Investment = ? WHERE Id = ?)"));
	if (!Insert.IsValid() ||
		!Insert.Bind(1, Investment) ||
		!Insert.Bind(2, ID) ||
		Insert.Step() != ESqliteStepResult::Done)
	{
		UE_LOG(LogRaid, Warning, TEXT("Failed to update investment on raid object"));
		return;
	}
}

void ARaid::RunNextEvent()
{
	TArray<URaidEvent*> PossibleEvents = AvailableEvents.FilterByPredicate([this](URaidEvent* Event) { return Event->CanRunEvent(); });
	int32 MaxRarity = 0;
	int32 TotalWeight = 0;
	for (const auto& Event : PossibleEvents)
	{
		MaxRarity = FMath::Max(MaxRarity, Event->Rarity);
	}

	if (!ensure(PossibleEvents.Num() > 0))
	{
		return;
	}

	++MaxRarity;
	for (const auto& Event : PossibleEvents)
	{
		TotalWeight += (MaxRarity - Event->Rarity);
	}

	int32 Selection = FMath::RandHelper(TotalWeight);
	int32 SelectedIndex = -1;
	do
	{
		++SelectedIndex;
		Selection -= (MaxRarity - PossibleEvents[SelectedIndex]->Rarity);
	} while (Selection > 0);
	PossibleEvents[SelectedIndex]->RunEvent();
}

void ARaid::OnRaidEventComplete_Implementation()
{
}

bool ARaid::SendTwitchMessage(FText Message)
{
	return UTwitchPluginBPLibrary::SendTwitchMessage(Chat, Message);
}

void ARaid::AddWinnings(int64 amount)
{
	Winnings += amount;
}

void ARaid::Complete_Implementation()
{
	State = ERaidState::Done;
	
	int64 LivingInvestment = 0;
	for (URaidParticipantComponent* Participant : Participants)
	{
		AChatPlayer* Player = CastChecked<AChatPlayer>(Participant->GetOwner());
		if (Participant->IsAlive())
		{
			LivingInvestment += Participant->GetInvestment();
		}
		else
		{
			Player->ForefeitLockedCaterium();
		}
		Player->UnlockCaterium();
	}

	for (URaidParticipantComponent* Participant : Participants)
	{
		AChatPlayer* Player = CastChecked<AChatPlayer>(Participant->GetOwner());
		if (Participant->IsAlive())
		{
			double Claim = (double)Participant->Investment / LivingInvestment;
			int64 PlayerWinnings = FMath::CeilToInt(Claim * Winnings);
			Participant->SetWinnings(PlayerWinnings);
			Player->AddCaterium(PlayerWinnings);
		}
		else
		{
			Player->AddCaterium(-Player->LockedCaterium);
		}
		Player->UnlockCaterium();
	}

	OnComplete.Broadcast(this);
	GetWorld()->DestroyActor(this);
}

bool ARaid::IsInProgress() const
{
	switch (State)
	{
	case ERaidState::Done:
	case ERaidState::NotStarted:
		return false;
	case ERaidState::Preparing:
	case ERaidState::Running:
	default:
		return true;
	}
}


bool ARaid::IsJoinable()
{
	switch (State)
	{
	case ERaidState::NotStarted:
	case ERaidState::Preparing:
		return true;
	case ERaidState::Done:
	case ERaidState::Running:
	default:
		return false;
	}
}

void ARaid::IsJoinable(AChatPlayer* Player, EJoinableOutput& Result, URaidParticipantComponent*& Participant)
{
	check(Player);
	if (!IsJoinable())
	{
		Result = EJoinableOutput::RaidNotJoinable;
	}
	else if (URaidParticipantComponent** Existing = ParticipantMap.Find(Player->ID); Existing)
	{
		Result = EJoinableOutput::AlreadyParticipating;
		Participant = *Existing;
	}
	else
	{
		Result = EJoinableOutput::RaidIsJoinable;
	}
}

void ARaid::GetLivingParticipants(TArray<URaidParticipantComponent*>& OutArray) const
{
	OutArray = Participants.FilterByPredicate(std::mem_fn(&URaidParticipantComponent::IsAlive));
}

URaidParticipantComponent* ARaid::GetActiveParticipant_Implementation() const
{
	return GetRandomParticipant();
}

URaidParticipantComponent* ARaid::GetRandomParticipant() const
{
	TArray<URaidParticipantComponent*> LivingParticipants;
	GetLivingParticipants(LivingParticipants);
	return LivingParticipants[FMath::RandHelper(LivingParticipants.Num())];
}

void ARaid::Join(AChatPlayer* Player, int32 investment)
{
	if (!IsJoinable())
	{
		return;
	}

	Investment += investment;
	URaidParticipantComponent* Participant = NewObject<URaidParticipantComponent>(Player, URaidParticipantComponent::StaticClass());
	Participant->Raid = this;
	Participant->Investment = investment;
	Participant->RegisterComponent();
	Participants.Add(Participant);
	ParticipantMap.Add(Player->ID, Participant);
}

bool ARaid::ReloadData()
{
	if (!EnsureTable())
	{
		return false;
	}

	auto Select = USqliteConnection::PrepareSimple(TEXT(R"(SELECT Id, Investment, Return FROM "Raid" WHERE Id = ?)"));
	if (!Select.IsValid() ||
		!Select.Bind(1, ID) ||
		Select.Step() != ESqliteStepResult::Data)
	{
		return false;
	}

	Select.AssignNextRowToObject(this);
	return true;
}
