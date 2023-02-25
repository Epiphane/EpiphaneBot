// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetBadgeImage.h"
#include "TwitchPlugin.h"
#include "GetTwitchEmoteSet.h"
#include "ImageHelpers.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
UGetBadgeImage::UGetBadgeImage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGetBadgeImage* UGetBadgeImage::GetBadgeImage(FTwitchBadge _Badge, FString BadgeScope)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UGetBadgeImage *EmoteFetchTask = NewObject<UGetBadgeImage>();
	EmoteFetchTask->Badge = _Badge;
	EmoteFetchTask->Scope = BadgeScope;
	return EmoteFetchTask;
}

void UGetBadgeImage::Activate()
{
	// Set the badge code if it isn't set by default (e.g. because the user uses the make struct thingy with one of the built-in enum values)
	if (Badge.RawBadgeCode.IsEmpty())
	{
		FString BadgeName = TEXT("");
		switch (Badge.Badge)
		{
		case ETwitchBadge::TB_ADMIN:
			BadgeName = "admin";
			break;
		case ETwitchBadge::TB_BITS:
			BadgeName = "bits";
			break;
		case ETwitchBadge::TB_BROADCASTER:
			BadgeName = "broadcaster";
			break;
		case ETwitchBadge::TB_GLOBALMOD:
			BadgeName = "global_mod";
			break;
		case ETwitchBadge::TB_MOD:
			BadgeName = "moderator";
			break;
		case ETwitchBadge::TB_SUBSCRIBER:
			BadgeName = "subscriber";
			break;
		case ETwitchBadge::TB_STAFF:
			BadgeName = "staff";
			break;
		case ETwitchBadge::TB_TURBO:
			BadgeName = "turbo";
			break;
		case ETwitchBadge::TB_PARTNER:
			BadgeName = "partner";
			break;
		case ETwitchBadge::TB_PREMIUM:
			BadgeName = "premium";
			break;
		case ETwitchBadge::TB_TWITCHBOT:
			BadgeName = "admin";
			break;
		case ETwitchBadge::TB_VIP:
			BadgeName = "vip";
			break;
		case ETwitchBadge::TB_SUBGIFTER:
			BadgeName = "sub-gifter";
			break;
		default:
			BadgeName = "unknown";
			break;
		}

		Badge.RawBadgeCode = BadgeName;
	}

	// Attempt to download or raed the JSON data containing all of the badges/URLs
	ScopeCachePath = FPaths::Combine(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()), FString::Printf(TEXT("twitch_badges/list_%s.json"), *Scope));
	if (GetMutableDefault<UTwitchRuntimeSettings>()->shouldCacheBadgeURLs && FPaths::FileExists(ScopeCachePath))
	{
		// Cache likely exists! Load it and kick off the parse (NOTE: using sparate function so we can 
		FString ReadData;
		if (FFileHelper::LoadFileToString(ReadData, *ScopeCachePath) && !ReadData.IsEmpty())
		{
			StartDownloadRequest(ReadData);
			return;
		}

		// Something went wrong - fall back and try to DL it again
	}

	// Fetch the JSON document containing all badge info
	CreateHTTPRequest();
	return;
}

void UGetBadgeImage::OnSuccessfulFetch(UTexture2D* Image)
{
	OnSuccess.Broadcast(Image, Badge);
}

void UGetBadgeImage::OnFailedFetch(UTexture2D* Image)
{
	OnFail.Broadcast(nullptr, Badge);
}

void UGetBadgeImage::StartDownloadRequest(FString BadgeData)
{
	// Attempt to turn the load the response's json
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(BadgeData);
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		TSharedPtr<FJsonObject> BadgeSets = JSONObject->GetObjectField(TEXT("badge_sets"));
		if (!BadgeSets)
		{
			goto badge_download_start_fail;
		}
		for (auto& Entry : BadgeSets->Values)
		{
			if (Entry.Key == Badge.RawBadgeCode)
			{
				// Located correct badge - try to find the appropriate version!
				TSharedPtr<FJsonObject> jBadge = Entry.Value->AsObject();
				if (!jBadge)
				{
					goto badge_download_start_fail;
				}

				TSharedPtr<FJsonObject> BadgeVersions = jBadge->GetObjectField(TEXT("versions"));
				if (!BadgeVersions)
				{
					goto badge_download_start_fail;
				}

				TSharedPtr<FJsonObject> BadgeVersion = BadgeVersions->GetObjectField(FString::FromInt(Badge.Version));
				if (!BadgeVersion)
				{
					// Badge version somehow does not exist?
					UE_LOG(LogTwitchWorks, Warning, TEXT("Version '%i' of badge '%s' is unknown!"), Badge.Version, *Badge.RawBadgeCode);
					goto badge_download_start_fail;
				}

				// Fetch the highest resolution image available (4x as of writing)
				FString ImageURL = BadgeVersion->GetStringField(TEXT("image_url_4x"));
				if (ImageURL.IsEmpty())
				{
					UE_LOG(LogTwitchWorks, Warning, TEXT("Version '%i' of badge '%s' is does not have a valid 4x image URL associated with it!"), Badge.Version, *Badge.RawBadgeCode);
					goto badge_download_start_fail;
				}

				// Now tha twe have the image URL, download it (and pull it from the cache if the settings permit us to)
				FImageDownloader* ImageRequest = new FImageDownloader();
				ImageRequest->OnSuccess.BindUObject(this, &UGetBadgeImage::OnSuccessfulFetch);
				ImageRequest->OnFail.BindUObject(this, &UGetBadgeImage::OnFailedFetch);
				ImageRequest->GetImage(ImageURL, GetMutableDefault<UTwitchRuntimeSettings>()->shouldCacheBadgeImages, FString::Printf(TEXT("/twitch_badges/twitch_badge_%s_%i.tbadge"), *Badge.RawBadgeCode, Badge.Version));
				
				// Return to avoid hitting the _fall-through_ error broadcast.
				return;
			}
		}
	}

badge_download_start_fail:
	// If we got here, something went horribly wrong -> fail the request!
	OnFail.Broadcast(nullptr, Badge);
	
}

void UGetBadgeImage::CreateHTTPRequest()
{
	FString BadgeURL = TEXT("");
	// The "global" scope is a special one that sends us to https://badges.twitch.tv/v1/badges/global/display, the other scopes are assumed to be channel-specific (where the scope is the channel's ID)
	if (Scope == TEXT("global"))
	{
		BadgeURL = TEXT("https://badges.twitch.tv/v1/badges/global/display");
	}
	else
	{
		BadgeURL = FString::Printf(TEXT("https://badges.twitch.tv/v1/badges/channels/%s/display"), *Scope);
	}

	// Create an HTTP request for the download
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);

	// Request the channels the user is following - passing along the limit of 100.
	HttpRequest->SetURL(BadgeURL);

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetBadgeImage::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void UGetBadgeImage::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0)
	{
		// Something went wrong getting the HTTP request - log out an error!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to get twitch badge list!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\n%s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Create a blank image to avoid possible nullptr issues and call the OnFail delegate
		OnFail.Broadcast(nullptr, Badge);
		return;
	}

	// Assume response is successful - we won't parse here, but we do kind of want to save it to avoid having to add a save param to the function that does parse it.
	// Pretty much - cache if necissary
	FString ResponseBody = HttpResponse->GetContentAsString();
	if (GetMutableDefault<UTwitchRuntimeSettings>()->shouldCacheBadgeURLs && !ScopeCachePath.IsEmpty())
	{
		FFileHelper::SaveStringToFile(ResponseBody, *ScopeCachePath);
	}

	// Hand the data off for further processing so we can download the actual image itself
	StartDownloadRequest(ResponseBody);
	return;
}

