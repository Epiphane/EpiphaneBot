// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetEmoteTask.h"
#include "TwitchPlugin.h"
#include "GetTwitchEmoteSet.h"
#include "ImageHelpers.h"
#include "Json.h"

// Default Constructor
UGetEmoteTask::UGetEmoteTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGetEmoteTask* UGetEmoteTask::GetEmoticonImage(FTwitchEmoticon _Emoticon)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UGetEmoteTask *EmoteFetchTask = NewObject<UGetEmoteTask>();
	EmoteFetchTask->Emoticon = _Emoticon;
	return EmoteFetchTask;
}

void UGetEmoteTask::Activate()
{
	// Attempt to download the image using the image helpers shared image processing tools. This will either pull it from cache (if enabled in the settings)
	//         or download it from the URL asynchronously.
	// NOTE: Must use a pointer to avoid it going out of scope when the function exits; should now be kept in scope until the class gets destroyed (once the request is done)
	FImageDownloader* ImageRequest = new FImageDownloader();
	ImageRequest->OnSuccess.BindUObject(this, &UGetEmoteTask::OnSuccessfulFetch);
	ImageRequest->OnFail.BindUObject(this, &UGetEmoteTask::OnFailedFetch);
	ImageRequest->GetImage(FString::Printf(TEXT("https://static-cdn.jtvnw.net/emoticons/v2/%s/static/light/1.0"), *Emoticon.Id), GetMutableDefault<UTwitchRuntimeSettings>()->shouldCacheEmoteRequests, FString::Printf(TEXT("/twitch_emotes/twitch_emoji_%i_%s.tmote"), Emoticon.EmoteSet, *Emoticon.Code));
}

void UGetEmoteTask::OnSuccessfulFetch(UTexture2D* Image)
{
	OnSuccess.Broadcast(Image, Emoticon);
}

void UGetEmoteTask::OnFailedFetch(UTexture2D* Image)
{
	OnFail.Broadcast(nullptr, Emoticon);
}

