// Fill out your copyright notice in the Description page of Project Settings.


#include "EpiGameInstance.h"
#include "EpiUserDataSubsystem.h"

void UEpiGameInstance::Init()
{
    UEpiUserDataSubsystem* UserData = GetSubsystem<UEpiUserDataSubsystem>();
    if (ensure(IsValid(UserData)))
    {
        UserData->GetUserCreatedEvent().AddDynamic(this, &UEpiGameInstance::OnNewUserCreated);
    }
}

void UEpiGameInstance::OnNewUserCreated(int32 ID, FString Name)
{
    OnNewPlayerJoined.Broadcast(Name, ID);
}
