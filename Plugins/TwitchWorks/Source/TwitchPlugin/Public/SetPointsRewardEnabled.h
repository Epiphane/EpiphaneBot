// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "SetPointsRewardEnabled.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTwitchSetPointsRewardEnabledDelegate);

/**
*  Task for enabling/disabling a channel points reward
*/
UCLASS()
class TWITCHPLUGIN_API USetPointsRewardEnabled : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Switches the enabled/disabled status of a custom channel points reward. Note that it must have been created by the same client ID/application for this to work.
	*
	* @param Chat The user/channel to use. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	* @param RewardID The ID of the reward you'd like to update.
	* @param bEnabled Whether or not this reward is enabled/should be shown to users
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Channel Points Reward Enabled - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static USetPointsRewardEnabled *SetPointsRewardEnabled(UTwitchChatConnector* Chat, FString RewardID, bool bEnabled);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchSetPointsRewardEnabledDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchSetPointsRewardEnabledDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The ID of the reward that should be updated
	FString RewardID;
	
	// Whether or not the reward should be set as enabled
	bool bIsEnabled;

	// The active chat connection that we can pull user info from
	UTwitchChatConnector* Chat = nullptr;
};