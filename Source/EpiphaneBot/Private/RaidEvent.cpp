// Fill out your copyright notice in the Description page of Project Settings.

#include "RaidEvent.h"
#include "TwitchPluginBPLibrary.h"
#include "Raid.h"

// Sets default values
ARaidEvent::ARaidEvent()
{
}

bool ARaidEvent::CanRunEvent_Implementation()
{
    return true;
}

void ARaidEvent::RunEvent_Implementation()
{
    MarkComplete();
}

void ARaidEvent::AddWinnings(float Amount)
{
    Raid->AddWinnings(FMath::CeilToInt64(Amount));
}

bool ARaidEvent::SendTwitchMessage(FText Message)
{
    return UTwitchPluginBPLibrary::SendTwitchMessage(Chat, Message);
}

void ARaidEvent::MarkComplete()
{
    OnComplete.Execute();
}

