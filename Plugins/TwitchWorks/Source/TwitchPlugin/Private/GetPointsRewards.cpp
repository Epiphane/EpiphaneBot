// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetPointsRewards.h"
#include "TwitchPlugin.h"
#include "ImageHelpers.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
UGetPointsRewards::UGetPointsRewards(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGetPointsRewards* UGetPointsRewards::GetPointsRewards(UTwitchChatConnector* InChat, bool bInFilterManageable)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UGetPointsRewards *GetPointsRewardsTask = NewObject<UGetPointsRewards>();
	GetPointsRewardsTask->Chat = InChat;
	GetPointsRewardsTask->bInFilterManageable = bInFilterManageable;
	return GetPointsRewardsTask;
}

void UGetPointsRewards::Activate()
{
	// Ensure the passed in chat was valid
	if (!Chat)
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Chat passed in to GetPointsRewards was invalid!"));
		OnFail.Broadcast(FRewardsMapWrapper());
		return;
	}

	
	// Create a HTTP GET request for the passed in chat
	FString RequestURL = FString::Printf(TEXT("https://api.twitch.tv/helix/channel_points/custom_rewards?broadcaster_id=%s"), *(Chat->ConnectedUserCreds.UserId));
	if (bInFilterManageable)
	{
		// Show only the ones which can be managed
		RequestURL += TEXT("&only_manageable_rewards=true");
	}
	
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Chat->ConnectedUserCreds.OAuthToken));
	HttpRequest->SetURL(RequestURL);

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetPointsRewards::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void UGetPointsRewards::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0 || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error to help debugging!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to execute points management request!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\nResponse Body: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Execute the failure delegate with default parameters
		OnFail.Broadcast(FRewardsMapWrapper());
		return;
	}

	// Log the response verbosely to help debug any issues this experimental node might have
	UE_LOG(LogTwitchWorks, Verbose, TEXT("GetPointsRewards Response: %s"), *HttpResponse->GetContentAsString());

	// Parse the response's JSON
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* jDataPtr = nullptr;
		if (!JSONObject->TryGetArrayField(TEXT("data"), jDataPtr) || !jDataPtr)
		{
			// Unable to extract the required array: this constitutes a failure!
			UE_LOG(LogTwitchWorks, Error, TEXT("Unable to parse API response!"));
			OnFail.Broadcast(FRewardsMapWrapper());
			return;
		}

		// Dereference the pointer now we know it's valid; this makes it easier to work with
		TArray<TSharedPtr<FJsonValue>> jData = *jDataPtr;

		// Parse the JSON containinng all of the redemptions into a BP-friendly struct array and succeed the request
		TMap<FString, FTwitchPointsReward> Rewards;
		for (auto &jReward : jData)
		{
			Rewards.Add(jReward->AsObject()->GetStringField(TEXT("id")), FTwitchPointsReward::FromJSON(jReward->AsObject()));
		}

		FRewardsMapWrapper RewardsWrapper;
		RewardsWrapper.Rewards = Rewards;
		OnSuccess.Broadcast(RewardsWrapper);
	}
}
