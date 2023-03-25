// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatOutcomePickers.h"

void UFloatOutcomePickers::TwoFloatOutcome(float Input, float Threshold, E2FloatOutcome& Result)
{
    if (Input < Threshold)
    {
        Result = E2FloatOutcome::Low;
    }
    else
    {
        Result = E2FloatOutcome::High;
    }
}

void UFloatOutcomePickers::ThreeFloatOutcome(float Input, float Threshold1, float Threshold2, E3FloatOutcome& Result)
{
    if (Input < Threshold1)
    {
        Result = E3FloatOutcome::Low;
    }
    else if (Input < Threshold2)
    {
        Result = E3FloatOutcome::Middle;
    }
    else
    {
        Result = E3FloatOutcome::High;
    }
}

void UFloatOutcomePickers::FourFloatOutcome(float Input, float Threshold1, float Threshold2, float Threshold3, E4FloatOutcome& Result)
{
    if (Input < Threshold1)
    {
        Result = E4FloatOutcome::Low;
    }
    else if (Input < Threshold2)
    {
        Result = E4FloatOutcome::MidLow;
    }
    else if (Input < Threshold3)
    {
        Result = E4FloatOutcome::MidHigh;
    }
    else
    {
        Result = E4FloatOutcome::High;
    }
}
