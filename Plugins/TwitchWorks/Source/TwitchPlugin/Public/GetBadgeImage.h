// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"
#include "TwitchRuntimeSettings.h"
#include "ImageHelpers.h"
#include "TwitchChatConnector.h"
#include "Interfaces/IHttpRequest.h"

#include "GetBadgeImage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwitchGetBadgeImageDelegate, UTexture2D*, BadgeImage, FTwitchBadge, BadgeData);

/**
*  Task for downloading a twitch emote.
*/
UCLASS()
class TWITCHPLUGIN_API UGetBadgeImage : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Fetches a twitch emoticon's image.
	*
	* @param Emoticon The twitch emoticon to fetch.
	* @param BadgeScope The scope in which to look; might want to replace the default of "global" with a channel ID for channel-specific badges.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetBadgeImage *GetBadgeImage(FTwitchBadge Badge, FString BadgeScope = TEXT("global"));

	virtual void Activate() override;

public:
	// Delegate that gets called when the plugin has successfully fetched the badge's image.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetBadgeImageDelegate OnSuccess;

	// Delegate that gets called when something went wrong while fetching the badge's image.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetBadgeImageDelegate OnFail;

private:
	// Handler for the image request callback.
	void OnSuccessfulFetch(UTexture2D *Image);
	void OnFailedFetch(UTexture2D* Image);

private:
	// Helper function to parse the JSON storing all badges, to find the relevant badge and to (down)load it.
	void StartDownloadRequest(FString BadgeData);

	// Helper function to download the document that contains all of the badge URLs
	void CreateHTTPRequest();

	// Helper function that processes the HTTP response we got when requesting the badge URLs
	void HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The badge to fetch
	FTwitchBadge Badge;

	// The scope to fetch badges in (either "global" or the channel ID)
	FString Scope;

	// The path where we cache the JSON document that has the URLs for all badges (NOTE: global variable to avoid code duplication of code that generates the asolute path from the scope)
	FString ScopeCachePath;
};