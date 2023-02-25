// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "DeletePointsReward.h"
#include "TwitchPlugin.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
UDeletePointsReward::UDeletePointsReward(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UDeletePointsReward* UDeletePointsReward::DeletePointsReward(UTwitchChatConnector *InChat, FString InRewardID)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UDeletePointsReward *DeletePointsRewardTask = NewObject<UDeletePointsReward>();
	DeletePointsRewardTask->RewardID = InRewardID;
	DeletePointsRewardTask->Chat = InChat;
	return DeletePointsRewardTask;
}

void UDeletePointsReward::Activate()
{
	// Ensure the passed in chat was valid
	if (!Chat)
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Chat passed in to DeletePointsReward was invalid!"));
		OnFail.Broadcast();
		return;
	}
	// Create a HTTP DELETE request for the current user's channel
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("DELETE"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Chat->ConnectedUserCreds.OAuthToken));
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/channel_points/custom_rewards?broadcaster_id=%s&id=%s"), *(Chat->ConnectedUserCreds.UserId), *RewardID));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDeletePointsReward::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void UDeletePointsReward::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error to help debugging!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to execute points deletion request!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\nResponse Body: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Execute the failure delegate with default parameters
		OnFail.Broadcast();
		return;
	}

	// If not, we succeeded - for proper responses no response data is provided, so no need to parse it either (yay!)
	OnSuccess.Broadcast();
	return;
}

