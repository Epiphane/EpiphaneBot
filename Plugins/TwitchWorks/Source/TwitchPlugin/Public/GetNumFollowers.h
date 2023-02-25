// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "TwitchChatConnector.h"
#include "GetNumFollowers.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetNumFollowersDelegate, int, NumFollowers);

/**
 * Task class for getting the number of followers.
 */
UCLASS()
class TWITCHPLUGIN_API UGetNumFollowers : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Gets the number of followers for a given channel
	*
	* @param Chat The chat to use as authorization for the request to twitch.
	* @param User The user that that you want to check.
	*
	* @return The number of followers.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetNumFollowers *GetTwitchNumFollowers(UTwitchChatConnector *Chat, FTwitchUser User);

	// Actually starts the logic (+ that way we have access to failure delegates)
	virtual void Activate() override;

public:
	// Callback delegate that gets called when we successfully retrieved the number of followers
	UPROPERTY(BlueprintAssignable)
	FTwitchGetNumFollowersDelegate OnSuccess;

	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetNumFollowersDelegate OnFail;

public:
	// Internal Helper Function - Creates a stream lookup HTTP request.
	void CreateHTTPRequest(FString OAuthToken, FString UserId);

private:
	// Internal Helper Function - Handler for stream lookup HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
private:
	// Copy of passed in variables so we can access them from activate
	UTwitchChatConnector* Chat;
	FTwitchUser User;
};