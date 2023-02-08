// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Raid.generated.h"

UENUM()
enum ERaidState
{
	NotStarted,
	Preparing,
	Running,
	Cooldown,
	Done,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaidCompleteDelegate, ARaid*, Raid);

UCLASS()
class EPIPHANEBOT_API ARaid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARaid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ERaidState GetState() const { return State; }
	FOnRaidCompleteDelegate GetOnRaidCompleteDelegate() const { return OnComplete; }

private:
	ERaidState State;

	UPROPERTY(BlueprintAssignable)
	FOnRaidCompleteDelegate OnComplete;

};
