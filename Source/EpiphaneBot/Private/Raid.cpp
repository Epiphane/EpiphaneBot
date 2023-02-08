// Fill out your copyright notice in the Description page of Project Settings.


#include "Raid.h"

// Sets default values
ARaid::ARaid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	State = ERaidState::NotStarted;
}

// Called when the game starts or when spawned
void ARaid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARaid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

