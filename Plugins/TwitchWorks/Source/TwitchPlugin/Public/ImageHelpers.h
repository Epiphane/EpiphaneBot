// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GetTwitchEmoteSet.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"
#include "TwitchRuntimeSettings.h"


DECLARE_DELEGATE_OneParam(FImageFetchResponse, UTexture2D*);

/**
 * 
 */
class TWITCHPLUGIN_API FImageDownloader
{
public:
	/*
	 * This function will execute a HTTP request to the backend using the provided parameters.
	 *
	 * @param URL The URL to the image lives at
	 * @param bAllowCache Whether or not to load from/save images to cache.
	 * @param CachePath The directory, file name and extension of the cache file belonging to this image; relative to FPaths::ProjectSavedDir()
	 */
	void GetImage(FString URL, bool bAllowCache = false, FString CachePath = TEXT(""));

public:
	FImageFetchResponse OnSuccess;
	FImageFetchResponse OnFail;

private:
	// Helper function for starting the image download HTTP request - this way we can still start it if the cache load fails (due to a corrupted image)
	void StartHttpRequest(FString URL);

	void HandleHTTPResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UTexture2D* byteArrayToTexture(TArray<uint8> ImageData);

private:
	// Copies of variables passed in to GetImage so we can reference them on the HTTP response
	bool bAllowCache;
	FString FullCachePath;

};
