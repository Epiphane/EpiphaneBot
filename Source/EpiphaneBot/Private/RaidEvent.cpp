// Fill out your copyright notice in the Description page of Project Settings.

#include "RaidEvent.h"
#include "TwitchPluginBPLibrary.h"
#include "Raid.h"

void URaidEvent::SetRaid(ARaid* InRaid)
{
    Raid = InRaid;
}

int32 URaidEvent::GetRarity_Implementation()
{
    return DefaultRarity;
}

bool URaidEvent::CanRunEvent_Implementation()
{
    return bEnabled;
}

void URaidEvent::RunEvent_Implementation()
{
    MarkComplete();
}

void URaidEvent::AddWinnings(float Amount)
{
    Raid->AddWinnings(FMath::CeilToInt64(Amount));
}

bool URaidEvent::SendTwitchMessage(FText Message)
{
    return UTwitchPluginBPLibrary::SendTwitchMessage(Chat, Message);
}

void URaidEvent::MarkComplete()
{
    OnComplete.Broadcast();
}

