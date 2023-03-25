// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FloatOutcomePickers.generated.h"

UENUM()
enum class E2FloatOutcome : uint8
{
	Low = 0,
	High,
};

UENUM()
enum class E3FloatOutcome : uint8
{
	Low = 0,
	Middle,
	High,
};

UENUM()
enum class E4FloatOutcome : uint8
{
	Low = 0,
	MidLow,
	MidHigh,
	High,
};

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API UFloatOutcomePickers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Float Outcome", Meta = (ExpandEnumAsExecs = "Result"))
	static void TwoFloatOutcome(float Input, float Threshold, E2FloatOutcome& Result);

	UFUNCTION(BlueprintCallable, Category = "Float Outcome", Meta = (ExpandEnumAsExecs = "Result"))
	static void ThreeFloatOutcome(float Input,  float Threshold1, float Threshold2, E3FloatOutcome& Result);

	UFUNCTION(BlueprintCallable, Category = "Float Outcome", Meta = (ExpandEnumAsExecs = "Result"))
	static void FourFloatOutcome(float Input, float Threshold1, float Threshold2, float Threshold3, E4FloatOutcome& Result);
};
