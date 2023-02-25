// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "SetPointsRewardEnabled.h"
#include "TwitchPlugin.h"
#include "ImageHelpers.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
USetPointsRewardEnabled::USetPointsRewardEnabled(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USetPointsRewardEnabled* USetPointsRewardEnabled::SetPointsRewardEnabled(UTwitchChatConnector* InChat, FString InRewardID, bool bInEnabled)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	USetPointsRewardEnabled *SetPointsRewardEnabledTask = NewObject<USetPointsRewardEnabled>();
	SetPointsRewardEnabledTask->Chat = InChat;
	SetPointsRewardEnabledTask->RewardID = InRewardID;
	SetPointsRewardEnabledTask->bIsEnabled = bInEnabled;
	return SetPointsRewardEnabledTask;
}

void USetPointsRewardEnabled::Activate()
{
	// Ensure the passed in chat was valid
	if (!Chat)
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Chat passed in to SetPointsRewardEnabled was invalid!"));
		OnFail.Broadcast();
		return;
	}
	// Create a HTTP PATCH request for the current user's channel
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("PATCH"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Chat->ConnectedUserCreds.OAuthToken));
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/channel_points/custom_rewards?broadcaster_id=%s&id=%s"), *(Chat->ConnectedUserCreds.UserId), *RewardID));

	// Attach the passed in (updated) reward configuration
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetBoolField(TEXT("is_enabled"), bIsEnabled);
	FString RequestBody;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&RequestBody);
	HttpRequest->SetContentAsString(RequestBody);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &USetPointsRewardEnabled::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void USetPointsRewardEnabled::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0 || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error to help debugging!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to execute points reward enabled update request!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\nResponse Body: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Execute the failure delegate with default parameters
		OnFail.Broadcast();
		return;
	}

	// Log the response verbosely to help debug any issues this experimental node might have
	UE_LOG(LogTwitchWorks, Verbose, TEXT("SetPointsRewardEnabled Response: %s"), *HttpResponse->GetContentAsString());

	// NOTE: The ID shouldn't/doesn't change, so we don't need to parse it - just succeed the request if it returns a non-error HTTP response code
	OnSuccess.Broadcast();
}

