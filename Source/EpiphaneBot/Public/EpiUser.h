// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EpiUserDataSubsystem.h"
#include "EpiUser.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserCateriumChangedDelegate, int32, NewCaterium);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserPrestigeChangedDelegate, int32, NewPrestige);

UCLASS()
class EPIPHANEBOT_API AEpiUser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEpiUser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void OnCateriumChanged(int32 ID, int32 NewCaterium);

	UFUNCTION()
	void OnPrestigeChanged(int32 ID, int32 NewPrestige);

protected:
	UPROPERTY(BlueprintAssignable)
	FUserCateriumChangedDelegate OnCateriumChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUserPrestigeChangedDelegate OnPrestigeChangedDelegate;

public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	int32 GetCaterium() const { return Data.Caterium; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	FEpiUserData Data;
};
