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

	UFUNCTION(BlueprintCallable)
	FString GetName() const { return GetOwner<AChatPlayer>()->Name; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetInvestment() const { return Investment; }

	UFUNCTION(BlueprintCallable)
	void SetInvestment(int32 NewInvestment);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 NewHealth);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWinnings() const { return Winnings; }

	UFUNCTION(BlueprintCallable)
	void SetWinnings(int32 NewWinnings);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const { return Health > 0; }

	UFUNCTION(BlueprintCallable)
	void Kill() { SetHealth(0); }

	UPROPERTY()
	ARaid* Raid;

	UPROPERTY(BlueprintGetter = GetInvestment, BlueprintSetter = SetInvestment)
	int32 Investment = 0;

private:
	UPROPERTY(BlueprintGetter = GetHealth, BlueprintSetter = SetHealth)
	int32 Health = 100;

	UPROPERTY(BlueprintGetter = GetWinnings, BlueprintSetter = SetWinnings)
	int32 Winnings = 0;
};