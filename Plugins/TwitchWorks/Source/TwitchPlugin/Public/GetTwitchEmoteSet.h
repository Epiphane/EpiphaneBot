// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "GetTwitchEmoteSet.generated.h"

/*
 *	Holds a Twitch emoticon's metadata
 */
USTRUCT(BlueprintType)
struct FTwitchEmoticon
{
	GENERATED_BODY()
	
	// The ID of the emote set the emote is in
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	int32 EmoteSet;

	// The emoticon code / name
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString Code;

	// The emoticon ID
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString Id;
};

// Delegate for calling back blueprint success/failure with an emoticon set.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetEmoteSetDelegate, const TArray<FTwitchEmoticon>&, EmoteSet);

/**
 *  Async Task To Fetch The Emoticons In A Twitch Emote Set
 */
UCLASS()
class TWITCHPLUGIN_API UGetTwitchEmoteSet : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
	
public:
	/**
	* Enables twitch slow chat.
	*
	* @param Chat The connected chat (used for authentication credentials)
	* @param EmoteSet The ID of the emote set to get the emoticons in.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetTwitchEmoteSet *GetTwitchEmoteSet(class UTwitchChatConnector* Chat, int32 EmoteSet);

	virtual void Activate() override;
	
	// Gets called when we successfully fetch the emoticons
	UPROPERTY(BlueprintAssignable)
	FTwitchGetEmoteSetDelegate OnSuccess;
	// Gets called when an error occurs when we are fetching the emoticons
	UPROPERTY(BlueprintAssignable)
	FTwitchGetEmoteSetDelegate OnFail;
	
private:
	// Internal Helper Function - Creates the emote set HTTP request.
	void CreateHTTPRequest(int32 EmoteSet, FString OAuthToken);

	// Internal Helper Function - Handler for HTTP Request Callbacks
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// Local variable storing the emote set so we can access it from all class functions
	class UTwitchChatConnector* Chat;
	int32 EmoteSet;
};