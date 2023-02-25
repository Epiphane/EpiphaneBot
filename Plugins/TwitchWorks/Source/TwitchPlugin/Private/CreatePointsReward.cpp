// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "CreatePointsReward.h"
#include "TwitchPlugin.h"
#include "ImageHelpers.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
UCreatePointsReward::UCreatePointsReward(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), Reward()
{
}

UCreatePointsReward* UCreatePointsReward::CreatePointsReward(UTwitchChatConnector* InChat, FTwitchPointsReward InReward)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UCreatePointsReward *CreatePointsRewardTask = NewObject<UCreatePointsReward>();
	CreatePointsRewardTask->Chat = InChat;
	CreatePointsRewardTask->Reward = InReward;
	return CreatePointsRewardTask;
}

void UCreatePointsReward::Activate()
{
	// Ensure the passed in chat was valid
	if (!Chat)
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Chat passed in to CreatePointsReward was invalid!"));
		OnFail.Broadcast(TEXT(""));
		return;
	}
	// Create a HTTP POST request for the current user's channel
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Chat->ConnectedUserCreds.OAuthToken));
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/channel_points/custom_rewards?broadcaster_id=%s"), *(Chat->ConnectedUserCreds.UserId)));

	// Attach the passed in reward configuration
	FString RequestBody;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(Reward.ToJSON().ToSharedRef(), JsonWriter);
	HttpRequest->SetContentAsString(RequestBody);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UCreatePointsReward::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void UCreatePointsReward::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0 || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error to help debugging!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to execute points management request!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\nResponse Body: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Execute the failure delegate with default parameters
		OnFail.Broadcast(TEXT(""));
		return;
	}

	// Log the response verbosely to help debug any issues this experimental node might have
	UE_LOG(LogTwitchWorks, Verbose, TEXT("CreatePointsReward Response: %s"), *HttpResponse->GetContentAsString());

	// Parse the response's JSON
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		const TArray<TSharedPtr<FJsonValue>>* jDataPtr = nullptr;
		if (!JSONObject->TryGetArrayField(TEXT("data"), jDataPtr) || !jDataPtr)
		{
			// Unable to extract the required array: this constitutes a failure!
			UE_LOG(LogTwitchWorks, Error, TEXT("Unable to parse backend response!"));
			OnFail.Broadcast(TEXT(""));
			return;
		}

		// Dereference the pointer now we know it's valid; this makes it easier to work with
		TArray<TSharedPtr<FJsonValue>> jData = *jDataPtr;

		// Ensure only one element is preset (NOTE: documentation is somewhat ambiguous about if all rewards or just the newly created one is returned, but since we need the ID / checking titles seems somewhat
		// undesired due to potential title sharing/just not being great, we'll assume it's just an arary object with one entry. Include a log entry in case we're wrong
		if (jData.Num() != 1)
		{
			UE_LOG(LogTemp, Error, TEXT("Excpected one data entry. Instead received: %s"), *HttpResponse->GetContentAsString());
			OnFail.Broadcast(TEXT(""));
			return;
		}

		// Extract the reward's ID and succeed the request
		FString RewardID = jData[0]->AsObject()->GetStringField(TEXT("id"));
		OnSuccess.Broadcast(RewardID);
	}
}

