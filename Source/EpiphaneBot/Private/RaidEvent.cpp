// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidEvent.h"

// Sets default values
URaidEvent::URaidEvent()
{
}

bool URaidEvent::CanRunEvent_Implementation(ARaid* Raid)
{
    return true;
}

void URaidEvent::RunEvent_Implementation(ARaid* Raid)
{
    OnComplete.Execute();
}


