// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "TwitchChatConnector.h"
#include "GetIsStreaming.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwitchGetIsStreamingDelegate, FString, SteamTitle, int, NumberOfViewers);

/**
 * Task class for getting whether a user is streaming or not.
 */
UCLASS()
class TWITCHPLUGIN_API UGetIsStreaming : public UBlueprintAsyncActionBase
{
		GENERATED_UCLASS_BODY()

public:
	/**
	* Checks if the given user is currently streaming.
	*
	* @param Chat The chat to use as authorization for the request to twitch.
	* @param User The user that that you want to check.
	*
	* @return Whether the user is currently streaming.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetIsStreaming *GetTwitchIsStreaming(UTwitchChatConnector *Chat, FTwitchUser User);

public:
	// Callback delegate that gets called when the user is streaming.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetIsStreamingDelegate OnStreaming;

	// Callback delegate that gets called when the user is NOT streaming.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetIsStreamingDelegate OnNotStreaming;

	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetIsStreamingDelegate OnFail;

public:
	// Internal Helper Function - Creates a stream lookup HTTP request.
	void CreateHTTPRequest(FString OAuthToken, FString UserId);

private:
	// Internal Helper Function - Handler for stream lookup HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	
	
};