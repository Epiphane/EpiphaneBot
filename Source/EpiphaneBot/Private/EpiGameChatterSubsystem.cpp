// Fill out your copyright notice in the Description page of Project Settings.

#include "EpiGameChatterSubsystem.h"
#include "ChatPlayer.h"
#include "EpiGameInstance.h"

void UEpiGameChatterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    /*
    UEpiGameInstance* GameInstance = Cast<UEpiGameInstance>(GetWorld()->GetGameInstance());
    if (GameInstance)
    {
        GameInstance->GetOnNewPlayerJoined().AddDynamic(this, &UEpiGameChatterSubsystem::OnChatPlayerJoined);
    }
    */
}

/*
void UEpiGameChatterSubsystem::ProcessChanges(const TSet<FChatter>& NewChatters, const TSet<FChatter>& LeftChatters)
{
    for (const FChatter& Chatter : NewChatters)
    {
        if (AChatPlayer::Exists(Chatter.UserId))
        {
            OnChatterJoined.Broadcast(Chatter);
        }
    }

    for (const FChatter& Chatter : LeftChatters)
    {
        OnChatterLeft.Broadcast(Chatter);
    }
}

void UEpiGameChatterSubsystem::OnChatPlayerJoined(FString UserName, int64 ID)
{
    FChatter Entry{ ID, UserName };
    OnChatterJoined.Broadcast(Entry);
    ActiveChatters.Emplace(Entry);
}
*/
