// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "DoubleViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API UDoubleViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	virtual bool CaptureMouseOnLaunch() override;
};
