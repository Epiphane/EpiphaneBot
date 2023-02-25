// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "DeletePointsReward.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTwitchDeletePointsRewardDelegate);

/**
* Task for deleting a channel points reward
*/
UCLASS()
class TWITCHPLUGIN_API UDeletePointsReward : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Deletes a channel points reward belonging to your application's ID.
	*
	* @param Chat The user/channel to create the reward for. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	* @param RewardID The ID of the reward you'd like to remove
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Delete Channel Points Reward - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static UDeletePointsReward *DeletePointsReward(UTwitchChatConnector *Chat, FString RewardID);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchDeletePointsRewardDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchDeletePointsRewardDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The ID of the reward to remove.
	FString RewardID;

	// Chat used for the broadcaster ID
	UTwitchChatConnector* Chat = nullptr;
};