// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "UpdatePointsReward.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTwitchUpdatePointsRewardDelegate);

/**
*  Task for updating a channel points reward
*/
UCLASS()
class TWITCHPLUGIN_API UUpdatePointsReward : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Updates a channel points reward on the user's channel. Note that it must have been created by the same client ID/application for this to work.
	*
	* @param Chat The user/channel to use. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	* @param RewardID The ID of the reward you'd like to update.
	* @param Reward The new reward settings
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Channel Points Reward - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static UUpdatePointsReward *UpdatePointsReward(UTwitchChatConnector* Chat, FString RewardID, FTwitchPointsReward Reward);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchUpdatePointsRewardDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchUpdatePointsRewardDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The ID of the reward that should be updated
	FString RewardID;
	
	// The reward's new settings
	FTwitchPointsReward Reward;

	// The active chat connection that we can pull user info from
	UTwitchChatConnector* Chat = nullptr;
};