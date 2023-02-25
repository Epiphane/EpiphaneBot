// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "TwitchChatConnector.h"
#include "GetChannelViewers.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetChannelViewersDelegate, const TArray<FString>&, Viewers);

/**
 * Task class for getting the current chatters/logged in viewers of a channel.
 */
UCLASS()
class TWITCHPLUGIN_API UGetChannelViewers : public UBlueprintAsyncActionBase
{
		GENERATED_UCLASS_BODY()

public:
	/**
	* Gets all logged in viewers (-> chatters) of a given channel.
	*
	* @parameter ChannelId The name of the channel to get the viewers for.
	*
	* @return The usernames of all chatters/logged in viewers of the given channel
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetChannelViewers *GetTwitchChannelViewers(FString ChannelId);

public:
	// Callback delegate that gets called when the list is successfully retrieved.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetChannelViewersDelegate OnSuccess;

	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetChannelViewersDelegate OnFail;

public:
	// Internal Helper Function - Creates a subscription lookup HTTP request.
	void CreateHTTPRequest(FString ChannelName);

private:
	// Internal Helper Function - Handler for subscription lookup HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	
	
};