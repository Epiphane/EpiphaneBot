// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "GetRewardRedemptions.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetRewardRedemptionsDelegate, const TArray<FTwitchPointsRedemption>&, Redemptions);

/**
*  Task for getting the redemptions of a given channel points reward.
*/
UCLASS()
class TWITCHPLUGIN_API UGetRewardRedemptions : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Gets the users that have redeemed/"purchased" a given channel points reward. NOTE: Reward must be created by the same Client ID.
	* NOTE: 50-user limit by default; oldest first.
	*
	* @param Chat The user/channel to create the reward for. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	* @param RewardID The reward you'd like to check
	* @param bOnlyUnfulfilled Only check unfulfilled rewards
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Channel Points Reward Redemptions - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static UGetRewardRedemptions *GetRewardRedemptions(UTwitchChatConnector* Chat, FString RewardID, bool bOnlyUnfulfilled=true);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchGetRewardRedemptionsDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchGetRewardRedemptionsDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The reward to check
	FString RewardID;

	// Filter for/only show ones with UNFULFILLED status
	bool bOnlyUnfulfilled = true;

	// The active chat connection that we can pull user info from
	UTwitchChatConnector* Chat = nullptr;
};