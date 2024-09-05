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
	OnStateChanged.Broadcast(State);

}

ARaid* ARaid::CreateRaid(UWorld* worldContext, TSubclassOf<ARaid> RaidClass, UTwitchChatConnector* Chat)
{
	if (!ensure(RaidClass))
	{
		return nullptr;
	}

	if (!EnsureTable())
	{
		return nullptr;
	}

	int64 Id;
	{
		auto Insert = USqliteConnection::PrepareSimple(TEXT(R"(INSERT INTO "Raid" (Time) VALUES (datetime('now')) RETURNING Id)"));
		if (!Insert.IsValid() ||
			Insert.Step() != ESqliteStepResult::Data)
		{
			return nullptr;
		}

		TMap<FString, FSQLiteValue> Properties = Insert.ReadRow();
		FSQLiteValue* IdValue = Properties.Find("Id");
		if (IdValue == nullptr)
		{
			return nullptr;
		}

		if (IdValue->Type != ESqliteValueType::Integer)
		{
			return nullptr;
		}

		Id = IdValue->IntValue;
	}

	ARaid* RaidObject = worldContext->SpawnActorDeferred<ARaid>(RaidClass, FTransform::Identity);
	check(RaidObject != nullptr);
	RaidObject->ID = Id;
	RaidObject->Chat = Chat;
	RaidObject->FinishSpawning(FTransform::Identity);
	if (!RaidObject->ReloadData())
	{
		RaidObject->Destroy();
		return nullptr;
	}

	return RaidObject;
}

// Called when the game starts or when spawned
void ARaid::BeginPlay()
{
	Super::BeginPlay();

	ForEachComponent<URaidEvent>(false, [this](URaidEvent* Component) {
		Component->Raid = this;
		Component->Chat = Chat;
		Component->OnComplete.AddDynamic(this, &ARaid::OnRaidEventComplete);
	});
}

// Called every frame
void ARaid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARaid::BeginPreparing_Implementation()
{
	State = ERaidState::Preparing;
	OnStateChanged.Broadcast(State);
}

void ARaid::BeginRaid_Implementation()
{
	State = ERaidState::Running;
	ON_SCOPE_EXIT{ OnStateChanged.Broadcast(State); };
	Investment = 0;
	MaxInvestment = 0;
	for (const auto& participant : Participants)
	{
		int investment = participant->GetInvestment();
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
	TArray<URaidEvent*> PossibleEvents;
	GetComponents(PossibleEvents);
	PossibleEvents = PossibleEvents.FilterByPredicate([this](URaidEvent* Event) { return Event->IsEnabled() && Event->CanRunEvent(); });
	int32 MaxRarity = 0;
	int32 TotalWeight = 0;
	for (const auto& Event : PossibleEvents)
	{
		int Rarity = Event->GetRarity();
		if (Rarity <= 0) Rarity = Event->DefaultRarity;
		MaxRarity = FMath::Max(MaxRarity, Rarity);
	}

	if (!ensure(PossibleEvents.Num() > 0))
	{
		return;
	}

	++MaxRarity;
	for (const auto& Event : PossibleEvents)
	{
		int Rarity = Event->GetRarity();
		if (Rarity <= 0) Rarity = Event->DefaultRarity;
		TotalWeight += (MaxRarity - Rarity);
	}

	int32 Selection = FMath::RandHelper(TotalWeight);
	int32 SelectedIndex = -1;
	do
	{
		++SelectedIndex;
		Selection -= (MaxRarity - PossibleEvents[SelectedIndex]->GetRarity());
	} while (Selection >= 0);
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
	ON_SCOPE_EXIT{ OnStateChanged.Broadcast(State); };
	
	int64 LivingInvestment = 0;
	for (URaidParticipantComponent* Participant : Participants)
	{
		AChatPlayer* Player = CastChecked<AChatPlayer>(Participant->GetOwner());
		if (Participant->IsAlive())
		{
			LivingInvestment += Participant->GetInvestment();
			UE_LOG(LogTemp, Log, TEXT("Adding %d living investment from %s"), Participant->GetInvestment(), *Player->Execute_GetUserName(Player));
		}
		else
		{
			Player->Execute_ForefeitLockedCaterium(Player);
		}
		Player->Execute_UnlockCaterium(Player);
	}

	for (URaidParticipantComponent* Participant : Participants)
	{
		AChatPlayer* Player = CastChecked<AChatPlayer>(Participant->GetOwner());
		if (Participant->IsAlive())
		{
			double Claim = (double)Participant->GetInvestment() / LivingInvestment;
			int64 PlayerWinnings = FMath::CeilToInt(Claim * Winnings);
			Participant->SetWinnings(PlayerWinnings);
			Player->Execute_UnlockCaterium(Player);
			UE_LOG(LogTemp, Log, TEXT("Giving %d caterium to %s (claim = %f, investment = %f/%d"), PlayerWinnings, *Player->Execute_GetUserName(Player), Claim, Participant->GetInvestment(), Participant->GetInvestment());
			Player->Execute_AddCaterium(Player, PlayerWinnings);
		}
		else
		{
			Player->Execute_ForefeitLockedCaterium(Player);
		}

		Participant->GetOwner()->SetLifeSpan(10.0f);
	}

	OnComplete.Broadcast(this);
	SetLifeSpan(10.0f);
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

void ARaid::IsJoinable(TScriptInterface<IEpiUser> User, EJoinableOutput& Result, URaidParticipantComponent*& Participant)
{
	check(User);
	if (!IsJoinable())
	{
		Result = EJoinableOutput::RaidNotJoinable;
	}
	else if (URaidParticipantComponent** Existing = ParticipantMap.Find(User->GetID_Implementation()); Existing)
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

void ARaid::GetLivingInactiveParticipants(TArray<URaidParticipantComponent*>& OutArray) const
{
	URaidParticipantComponent* Active = GetActiveParticipant();
	OutArray = Participants.FilterByPredicate([Active](URaidParticipantComponent* Participant) {\
		return Participant->IsAlive() && Participant != Active;
	});
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

AChatPlayer* ARaid::Join(TScriptInterface<IEpiUser> User, TSubclassOf<AChatPlayer> Class, int32 Amount)
{
	if (!IsJoinable())
	{
		return nullptr;
	}

	AChatPlayer* Player = AChatPlayer::Spawn(this, Class, User);

	Investment += Amount;
	URaidParticipantComponent* Participant = NewObject<URaidParticipantComponent>(Player, URaidParticipantComponent::StaticClass());
	Participant->Raid = this;
	Participant->Investment = Amount;
	Participant->RegisterComponent();
	Participants.Add(Participant);
	ParticipantMap.Add(Player->GetID_Implementation(), Participant);
	
	OnPlayerJoined(Player, Amount);
	return Player;
}

void ARaid::OnPlayerJoined_Implementation(AChatPlayer* Player, int32 Amount)
{
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

	Select.AssignNextRowToObject(GetClass(), this);
	return true;
}
