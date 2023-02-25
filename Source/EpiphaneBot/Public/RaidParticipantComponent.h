// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChatPlayer.h"
#include "GameFramework/Actor.h"
#include "RaidParticipantComponent.generated.h"

class ARaid;
class AChatPlayer;

UCLASS(Within=ChatPlayer)
class URaidParticipantComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static bool EnsureTable();

public:
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetInvestment() { return Investment; }

	UFUNCTION(BlueprintCallable)
	void SetInvestment(int32 NewInvestment);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetHealth() { return Health; }

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 NewHealth);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() { return Health > 0; }

	UFUNCTION(BlueprintCallable)
	void Kill() { SetHealth(0); }

	UPROPERTY()
	ARaid* Raid;

	UPROPERTY(BlueprintGetter = GetInvestment, BlueprintSetter = SetInvestment)
	int32 Investment = 0;

	UPROPERTY(BlueprintGetter = GetHealth, BlueprintSetter = SetHealth)
	int32 Health = 100;
};