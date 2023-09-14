// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatterListSubsystem.h"
#include "TwitchPubSubConnection.h"

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
    GetWorld()->GetTimerManager().SetTimer(PollTimer, this, &UChatterListSubsystem::OnTimerActivate, 10.0f, true, 0.0f);
}

void UChatterListSubsystem::OnDisconnected()
{
    GetWorld()->GetTimerManager().ClearTimer(PollTimer);
}

void UChatterListSubsystem::OnTimerActivate()
{
    QueryTask->MakeRequest(Events->GetCredentials());
}

void UChatterListSubsystem::OnQuerySuccess(const TArray<FChatter>& Chatters)
{
    TSet<FChatter> NewChatters;
    TSet<FChatter> PrevChatters = ActiveChatters;
    ActiveChatters.Empty(ActiveChatters.Num());
    for (const FChatter& Chatter : Chatters)
    {
        if (PrevChatters.Contains(Chatter))
        {
            PrevChatters.Remove(Chatter);
        }
        else
        {
            NewChatters.Add(Chatter);
        }
        ActiveChatters.Add(Chatter);
    }

    ProcessChanges(NewChatters, PrevChatters);
}

void UChatterListSubsystem::OnQueryFail(const TArray<FChatter>& Viewers)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to get chatters"));
}

void UChatterListSubsystem::ProcessChanges(const TSet<FChatter>& NewChatters, const TSet<FChatter>& LeftChatters)
{
    for (const FChatter& Chatter : NewChatters)
    {
        OnChatterJoined.Broadcast(Chatter);
    }

    for (const FChatter& Chatter : LeftChatters)
    {
        OnChatterLeft.Broadcast(Chatter);
    }
}
