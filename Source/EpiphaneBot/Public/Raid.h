// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChatPlayer.h"
#include "RaidEvent.h"
#include "GameFramework/Actor.h"
#include "RaidParticipantComponent.h"
#include "Raid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRaid, Log, All);

UENUM()
enum class ERaidState: uint8
{
	NotStarted = 0,
	Preparing,
	Running,
	Done,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRaidCompleteDelegate, ARaid*, Raid);

UENUM(BlueprintType)
enum class EJoinableOutput :uint8
{
	AlreadyParticipating = 0,
	RaidIsJoinable,
	RaidNotJoinable,
};

UCLASS()
class EPIPHANEBOT_API ARaid : public AActor
{
	GENERATED_BODY()

public:
	static bool EnsureTable();
	
private:	
	// Sets default values for this actor's properties
	ARaid();

public:
	static ARaid* CreateRaid(UWorld* worldContext, TSubclassOf<ARaid> RaidClass, TArray<TSubclassOf<URaidEvent>> AvailableEvents, UTwitchChatConnector* Chat);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	void BeginPreparing();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginRaid();

	UFUNCTION(BlueprintCallable)
	void RunNextEvent();

	UFUNCTION(BlueprintNativeEvent)
	void OnRaidEventComplete();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Complete();

	UFUNCTION(BlueprintCallable)
	bool SendTwitchMessage(FText Message);

	UFUNCTION(BlueprintCallable)
	void AddWinnings(int64 Amount);

	UFUNCTION(BlueprintCallable)
	bool IsInProgress() const;

	bool IsJoinable();

	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "Result"))
	void IsJoinable(AChatPlayer* Player, EJoinableOutput& Result, URaidParticipantComponent*& Participant);

	ERaidState GetState() const { return State; }
	FOnRaidCompleteDelegate& GetOnRaidCompleteDelegate() { return OnComplete; }

	UFUNCTION(BlueprintCallable)
	void GetParticipants(TArray<URaidParticipantComponent*>& OutArray) const { OutArray = Participants; }

	UFUNCTION(BlueprintCallable)
	void GetLivingParticipants(TArray<URaidParticipantComponent*>& OutArray) const;

	UFUNCTION(BlueprintCallable)
	URaidParticipantComponent* GetParticipant(int32 Index) const { return Participants[Index]; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	URaidParticipantComponent* GetActiveParticipant() const;

	UFUNCTION(BlueprintCallable)
	URaidParticipantComponent* GetRandomParticipant() const;

	UFUNCTION(BlueprintCallable)
	void Join(AChatPlayer* Player, int32 Investment);

	float GetTimeBeforeNextRaid() const { return TimeBeforeNextRaid; }

private:
	bool ReloadData();

public:
	UPROPERTY()
	int64 ID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 Investment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 AverageInvestment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 MaxInvestment;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int64 Winnings;

	UPROPERTY()
	ERaidState State;

	UPROPERTY(EditDefaultsOnly, meta = (Category = "Raid"))
	float TimeBeforeNextRaid = 60.0f;

	UPROPERTY(BlueprintAssignable)
	FOnRaidCompleteDelegate OnComplete;

	UPROPERTY()
	TArray<URaidParticipantComponent*> Participants;

	UPROPERTY()
	TMap<int32, URaidParticipantComponent*> ParticipantMap;

	UPROPERTY()
	TArray<URaidEvent*> AvailableEvents;

	UPROPERTY(BlueprintReadOnly)
	UTwitchChatConnector* Chat;
};
