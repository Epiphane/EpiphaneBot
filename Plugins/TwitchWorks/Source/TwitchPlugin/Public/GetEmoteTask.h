// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GetTwitchEmoteSet.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"
#include "TwitchRuntimeSettings.h"
#include "ImageHelpers.h"


#include "GetEmoteTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwitchGetEmoticonDelegate, UTexture2D*, EmoticonImage, FTwitchEmoticon, EmoticonData);

/**
*  Task for downloading a twitch emote.
*/
UCLASS()
class TWITCHPLUGIN_API UGetEmoteTask : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Fetches a twitch emoticon's image.
	*
	* @param Emoticon The twitch emoticon to fetch.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetEmoteTask *GetEmoticonImage(FTwitchEmoticon Emoticon);

	virtual void Activate() override;

public:
	// Delegate that gets called when the plugin has successfully fetched an emoticon.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetEmoticonDelegate OnSuccess;
	// Delegate that gets called when something went wrong while fetching the emoticons.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetEmoticonDelegate OnFail;

private:
	// Handler for the image request callback.
	void OnSuccessfulFetch(UTexture2D *Image);
	void OnFailedFetch(UTexture2D* Image);

private:
	FTwitchEmoticon Emoticon;
};