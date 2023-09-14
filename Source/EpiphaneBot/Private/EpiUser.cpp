// Fill out your copyright notice in the Description page of Project Settings.

#include "EpiUser.h"
#include "EpiUserDataSubsystem.h"

// Sets default values
AEpiUser::AEpiUser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEpiUser::BeginPlay()
{
	Super::BeginPlay();
	ensure(Data.ID != 0);

	UEpiUserDataSubsystem* UserData = GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (ensure(IsValid(UserData)))
	{
#define BIND_PROPERTY_CHANGED(Property) F ## Property ## ChangedDelegate Property ## Delegate; \
	Property ## Delegate.BindDynamic(this, &AEpiUser::On ## Property ## Changed); \
	UserData->BindOn ## Property ## Changed(Data.ID, Property ## Delegate);

		BIND_PROPERTY_CHANGED(Caterium);
		BIND_PROPERTY_CHANGED(Prestige);

#undef BIND_PROPERTY_CHANGED
	}
}

void AEpiUser::OnCateriumChanged(int32, int32 NewCaterium)
{
	Data.Caterium = NewCaterium;
	OnCateriumChangedDelegate.Broadcast(NewCaterium);
}

void AEpiUser::OnPrestigeChanged(int32, int32 NewPrestige)
{
	Data.Prestige = NewPrestige;
	OnPrestigeChangedDelegate.Broadcast(NewPrestige);
}
