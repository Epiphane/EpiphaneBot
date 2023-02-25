// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "UpdateRedemptionStatus.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTwitchUpdateRedemptionStatusDelegate);

/**
* Task for updating a channnel points redemption (fulfillment) status
*/
UCLASS()
class TWITCHPLUGIN_API UUpdateRedemptionStatus : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Changes the fulfillment status of a redemption.
	*
	* @param Chat The user/channel to create the reward for. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	* @param RewardID The ID of the reward this redemption belongs to.
	* @param RedemptionID The ID of the redemption to update.
	* @param NewStatus The status you'd like to set the redemption to.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Channel Points Redemption Status - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static UUpdateRedemptionStatus *UpdateRedemptionStatus(UTwitchChatConnector *Chat, FString RedemptionID, FString RewardID, ETwitchRedemptionStatus NewStatus);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchUpdateRedemptionStatusDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchUpdateRedemptionStatusDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The ID of the reward and redemption to change
	FString RedemptionID;
	FString RewardID;

	// The status to change the redemption to
	ETwitchRedemptionStatus NewStatus = ETwitchRedemptionStatus::RS_CANCELED;

	// Chat used for the broadcaster ID
	UTwitchChatConnector* Chat = nullptr;
};