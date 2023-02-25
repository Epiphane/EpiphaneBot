// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "TwitchChatConnector.h"
#include "GetIsSubscribed.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTwitchGetIsSubscribedDelegate);

/**
 * Task class for getting whether a user is subscribed or not.
 */
UCLASS()
class TWITCHPLUGIN_API UGetIsSubscribed : public UBlueprintAsyncActionBase
{
		GENERATED_UCLASS_BODY()

public:
	/**
	* Checks if the given user is subscribed to a channel.
	*
	* @param Chat The chat to use as autherization for the request to twitch.
	* @param User The user that that you want to check. Must have permission with the Chat's credentials for this to work.
	* @parameter ChannelId The ID of the channel to check the subscription to.
	*
	* @return Whether the user is subscribed to that channel.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetIsSubscribed *GetTwitchIsSubscribed(UTwitchChatConnector *Chat, FTwitchUser User, FString ChannelId);

public:
	// Callback delegate that gets called when the user is indeed subscribed to the channel.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetIsSubscribedDelegate OnSubscribed;
	// Callback delegate that gets called when the user is not subscribed to the channel.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetIsSubscribedDelegate OnNotSubscribed;
	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetIsSubscribedDelegate OnFail;

public:
	// Internal Helper Function - Creates a subscription lookup HTTP request.
	void CreateHTTPRequest(FString OAuthToken, FString UserId, FString ChannelId);

private:
	// Internal Helper Function - Handler for subscription lookup HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	
	
};