// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetRewardRedemptions.h"
#include "TwitchPlugin.h"
#include "ImageHelpers.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
UGetRewardRedemptions::UGetRewardRedemptions(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGetRewardRedemptions* UGetRewardRedemptions::GetRewardRedemptions(UTwitchChatConnector* InChat, FString InRewardID, bool InOnlyUnfulfilled)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UGetRewardRedemptions *GetRewardRedemptionsTask = NewObject<UGetRewardRedemptions>();
	GetRewardRedemptionsTask->Chat = InChat;
	GetRewardRedemptionsTask->RewardID = InRewardID;
	GetRewardRedemptionsTask->bOnlyUnfulfilled = InOnlyUnfulfilled;
	return GetRewardRedemptionsTask;
}

void UGetRewardRedemptions::Activate()
{
	// Ensure the passed in chat was valid
	if (!Chat)
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Chat passed in to GetRewardRedemptions was invalid!"));
		OnFail.Broadcast(TArray<FTwitchPointsRedemption>());
		return;
	}

	
	// Create a HTTP GET request for the passed in user/channel
	FString RequestURL = FString::Printf(TEXT("https://api.twitch.tv/helix/channel_points/custom_rewards/redemptions?broadcaster_id=%s&reward_id=%s"), *(Chat->ConnectedUserCreds.UserId), *RewardID);
	if (bOnlyUnfulfilled)
	{
		// Show only unfulfilled ones
		RequestURL += TEXT("&status=UNFULFILLED");
	}
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Chat->ConnectedUserCreds.OAuthToken));
	HttpRequest->SetURL(RequestURL);

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetRewardRedemptions::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void UGetRewardRedemptions::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0 || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error to help debugging!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to execute points management request!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\nResponse Body: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Execute the failure delegate with default parameters
		OnFail.Broadcast(TArray<FTwitchPointsRedemption>());
		return;
	}

	// Log the response verbosely to help debug any issues this experimental node might have
	UE_LOG(LogTwitchWorks, Verbose, TEXT("GetRewardRedemptions Response: %s"), *HttpResponse->GetContentAsString());

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
			OnFail.Broadcast(TArray<FTwitchPointsRedemption>());
			return;
		}

		// Dereference the pointer now we know it's valid; this makes it easier to work with
		TArray<TSharedPtr<FJsonValue>> jData = *jDataPtr;

		// Parse the JSON containinng all of the redemptions into a BP-friendly struct array and succeed the request
		TArray<FTwitchPointsRedemption> Redemptions;
		for (auto &jRedemption : jData)
		{
			Redemptions.Add(FTwitchPointsRedemption::FromJSON(jRedemption->AsObject()));
		}

		OnSuccess.Broadcast(Redemptions);
	}
}

