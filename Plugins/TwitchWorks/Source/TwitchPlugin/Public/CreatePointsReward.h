// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "CreatePointsReward.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchCreatePointsRewardDelegate, FString, RewardID);

/**
*  Task for creating a new channel points reward.
*/
UCLASS()
class TWITCHPLUGIN_API UCreatePointsReward : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Creates a new channel points reward on the user's channel.
	*
	* NOTE: TwiWorks does not currently support points events that don't require text input.
	* We are still waiting for a new feature to release before implementing support for this, but hope to add it in the future.
	* NOTE: Requires latest backend version for the UserId credentials property & "channel:manage:redemptions" OAuth scope.
	*
	* @param Reward The reward to create
	* @param Chat The user/channel to create the reward for. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Channel Points Reward - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static UCreatePointsReward *CreatePointsReward(UTwitchChatConnector* Chat, FTwitchPointsReward Reward);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchCreatePointsRewardDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchCreatePointsRewardDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The reward to create
	FTwitchPointsReward Reward;

	// The active chat connection that we can pull user info from
	UTwitchChatConnector* Chat = nullptr;
};