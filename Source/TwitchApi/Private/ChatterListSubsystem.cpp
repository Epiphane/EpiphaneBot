// Fill out your copyright notice in the Description page of Project Settings.


#include "TwitchPubSubConnection.h"
#include "TwitchGetChatters.h"
#include "ChatterListSubsystem.h"

void UChatterListSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Events = Collection.InitializeDependency<UTwitchEventsSubsystem>();
    Events->GetOnConnected().AddDynamic(this, &UChatterListSubsystem::OnConnected);
    Events->GetOnDisconnected().AddDynamic(this, &UChatterListSubsystem::OnDisconnected);

    QueryTask = NewObject<UTwitchGetChatters>();
    QueryTask->OnSuccess.AddDynamic(this, &UChatterListSubsystem::OnQuerySuccess);
    QueryTask->OnFail.AddDynamic(this, &UChatterListSubsystem::OnQueryFail);
}

void UChatterListSubsystem::OnConnected(FString Channel)
{
    GetWorld()->GetTimerManager().SetTimer(PollTimer, this, &UChatterListSubsystem::OnTimerActivate, 10.0f, true);
}

void UChatterListSubsystem::OnDisconnected()
{
    GetWorld()->GetTimerManager().ClearTimer(PollTimer);
}

void UChatterListSubsystem::OnTimerActivate()
{
    QueryTask->MakeRequest(Events->GetCredentials());
}

void UChatterListSubsystem::OnQuerySuccess(const TArray<FString>& Viewers)
{
    FString joined = FString::Join(Viewers, TEXT(" | "));
    UE_LOG(LogTemp, Log, TEXT("Chatters: %s"), *joined);
}

void UChatterListSubsystem::OnQueryFail(const TArray<FString>& Viewers)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to get chatters"));
}
