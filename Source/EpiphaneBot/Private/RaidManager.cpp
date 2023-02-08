// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidManager.h"


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
    case ERaidState::Cooldown:
    case ERaidState::Preparing:
    case ERaidState::Running:
    default:
        return true;
    }
}

ARaid* URaidManager::StartRaid()
{
    check(CurrentRaid == nullptr);

    CurrentRaid = NewObject<ARaid>();
    CurrentRaid->GetOnRaidCompleteDelegate().AddDynamic(this, &URaidManager::OnRaidDone);
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

void URaidManager::OnRaidDone(ARaid* Raid)
{
}