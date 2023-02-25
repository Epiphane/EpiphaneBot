// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "PointsReward.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "GetPointsRewards.generated.h"

// Workaround for using multiple template arguments in a variac macro; UE4 will complain if we don't pass in one of its types, so the best way to work around is is just throwing the template in a wrapper.
//typedef TMap<FString, FTwitchPointsReward> FPointsRewardMap;
//#define PointsRewardMap TMap<FString, FTwitchPointsReward>
USTRUCT(BlueprintType)
struct FRewardsMapWrapper
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks")
	TMap<FString, FTwitchPointsReward> Rewards;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetPointsRewardsDelegate, FRewardsMapWrapper, Rewards);

/**
*  Task for getting the channel points rewards set up for a given channel.
*/
UCLASS()
class TWITCHPLUGIN_API UGetPointsRewards : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Gets the channel points rewards that the user has set up on their channel.
	*
	* @param Chat The user/channel to create the reward for. !WARNING! You must use the latest login version / set the UserId property IS REQUIRED!
	* @param bInFilterManageable Only return rewards the app has access to
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Channel Points Rewards - EXPERIMENTAL", BlueprintInternalUseOnly = "true"), Category = "TwitchWorks|EXPERIMENTAL")
	static UGetPointsRewards *GetPointsRewards(UTwitchChatConnector* Chat, bool bInFilterManageable =false);

	// Actually runs the BP code post binding delegates/when the BP actually wants it to run
	virtual void Activate() override;

public:
	// Delegate that gets called when the request succeeds
	UPROPERTY(BlueprintAssignable)
	FTwitchGetPointsRewardsDelegate OnSuccess;

	// Delegate that gets called when the request fails
	UPROPERTY(BlueprintAssignable)
	FTwitchGetPointsRewardsDelegate OnFail;

private:
	// Helper function that processes the HTTP and calls the appropriate delegate
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// Filter for/only show ones with UNFULFILLED status
	bool bInFilterManageable = true;

	// The active chat connection that we can pull user info from
	UTwitchChatConnector* Chat = nullptr;
};