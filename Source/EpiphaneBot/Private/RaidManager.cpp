// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidManager.h"

DEFINE_LOG_CATEGORY(LogRaidManager);

void URaidManager::Initialize(FSubsystemCollectionBase& Collection)
{
    AvailableEvents.Empty();
    for (TObjectIterator<UClass> It; It; ++It)
    {
        if (It->IsChildOf(URaidEvent::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
        {
            AvailableEvents.Add(*It);
        }
    }
}

bool URaidManager::IsRaidOngoing()
{
    if (!CurrentRaid)
    {
        return false;
    }

    ERaidState State = CurrentRaid->GetState();
    if (State == ERaidState::Done)
    {
        CurrentRaid = nullptr;
    }

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

ARaid* URaidManager::StartRaid(TSubclassOf<ARaid> RaidClass)
{
    check(CurrentRaid == nullptr);

    CurrentRaid = ARaid::CreateRaid(GetWorld(), RaidClass, AvailableEvents, Chat);
    CurrentRaid->GetOnRaidCompleteDelegate().AddDynamic(this, &URaidManager::OnRaidDone);
    CurrentRaid->BeginPreparing();
    return CurrentRaid;
}

ARaid* URaidManager::GetCurrentRaid()
{
    if (CurrentRaid && CurrentRaid->GetState() == ERaidState::Done)
    {
        CurrentRaid = nullptr;
    }

    return CurrentRaid;
}

void URaidManager::GetRaidEligibility(AChatPlayer* Player, ERaidEligibility& Result, ARaid*& Raid, URaidParticipantComponent*& Participant, float& Cooldown)
{
    Raid = CurrentRaid;
    Cooldown = GetWorld()->GetTimerManager().GetTimerRemaining(RaidCooldownTimer);
    if (Cooldown > 0)
    {
        Result = ERaidEligibility::Cooldown;
    }
    else if (CurrentRaid == nullptr)
    {
        Result = ERaidEligibility::NoCurrentRaid;
    }
    else
    {
        ERaidState state = CurrentRaid->GetState();
        EJoinableOutput Joinable;
        CurrentRaid->IsJoinable(Player, Joinable, Participant);
        if (Joinable == EJoinableOutput::RaidIsJoinable)
        {
            Result = ERaidEligibility::RaidIsJoinable;
        }
        else if(Joinable == EJoinableOutput::RaidNotJoinable)
        {
            Result = ERaidEligibility::RaidIsOngoing;
        }
        else if (Joinable == EJoinableOutput::AlreadyParticipating)
        {
            Result = ERaidEligibility::AlreadyParticipating;
        }
    }
}

void URaidManager::OnRaidDone(ARaid* Raid)
{
    CurrentRaid = nullptr;
    GetWorld()->GetTimerManager().SetTimer(RaidCooldownTimer, this, &URaidManager::OnCooldownOver, Raid->GetTimeBeforeNextRaid());
    UE_LOG(LogRaidManager, Log, TEXT("Raid complete"));
}

void URaidManager::OnCooldownOver()
{
    OnRaidCooldownOver.Broadcast();
}