// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "PointsReward.generated.h"


/*
 * Twitch channel points reward structure. Used for creating and updating channel points related functionality.
 */
USTRUCT(BlueprintType)
struct FTwitchPointsReward
{
	GENERATED_USTRUCT_BODY()

public:
	/*
	 * The name/title of the reward.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString Title;

	/*
	 * The number of channel points this reward should cost.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	int Cost;

	/*
	 * Whether or not this reward should be shown to users.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bIsEnabled = true;

	/*
	 * (Optional) prompt text to show.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString PromptText;

	
	/*
	 * Background color to use for the reward.
	 * NOTE: Alpha channel is ignored
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FColor BackgroundColor;

	/*
	 * (Optional) Maximum number of times this can be redeemed per stream
	 * Set to -1 to disable.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	int MaxPerSteam = -1;

	/*
	 * (Optional) Maximum number of times a user can be redeemed per stream
	 * Set to -1 to disable.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	int MaxPerUserPerSteam;

	/*
	 * (Optional) Cooldown / minimum time (in s) between two reward redemptions
	 * Set to -1 to disable.
	 * NOTE: Must be at least 1 minute / 60 seconds!
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	int GlobalCooldown;

	/*
	 * (Optional) Whether or not the reward should be marked as fulfilled immediately after a use rtriggers it
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bInstantFulfillment;

public:
	TSharedPtr<FJsonObject> ToJSON();

	static FTwitchPointsReward FromJSON(TSharedPtr<FJsonObject> JSONObject);
	
};


// Twitch Badges
UENUM(BlueprintType)
enum class ETwitchRedemptionStatus : uint8
{
	RS_UNFULFILLED,
	RS_FULFILLED,
	RS_CANCELED
};



/*
 * Twitch channel points reward structure. Used for creating and updating channel points related functionality.
 */
USTRUCT(BlueprintType)
struct FTwitchPointsRedemption
{
	GENERATED_USTRUCT_BODY()

public:
	/*
	 * The name/title of the reward.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString RedemptionID;

	/*
	 * The number of channel points this reward should cost.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	ETwitchRedemptionStatus Status;

	/*
	 * (Optional) prompt text to show.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString RedeemerUsername;

	
	/*
	 * Background color to use for the reward.
	 */
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString RedeemerID;
public:
	static FTwitchPointsRedemption FromJSON(TSharedPtr<FJsonObject> JSONObject);
};



