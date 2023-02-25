// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "UpdateRedemptionStatus.h"
#include "TwitchPlugin.h"
#include "Runtime/Core/Public/Core.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"

// Default Constructor
UUpdateRedemptionStatus::UUpdateRedemptionStatus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UUpdateRedemptionStatus* UUpdateRedemptionStatus::UpdateRedemptionStatus(UTwitchChatConnector* InChat, FString InRedemptionID, FString InRewardID, ETwitchRedemptionStatus InNewStatus)
{
	// Create and set up an instance of this class and then return it so the blueprint callbacks work
	UUpdateRedemptionStatus *UpdateRedemptionStatusTask = NewObject<UUpdateRedemptionStatus>();
	UpdateRedemptionStatusTask->RedemptionID = InRedemptionID;
	UpdateRedemptionStatusTask->RewardID = InRewardID;
	UpdateRedemptionStatusTask->Chat = InChat;
	UpdateRedemptionStatusTask->NewStatus = InNewStatus;
	return UpdateRedemptionStatusTask;
}

void UUpdateRedemptionStatus::Activate()
{
	// Ensure the passed in chat was valid
	if (!Chat)
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Chat passed in to UpdateRedemptionStatus was invalid!"));
		OnFail.Broadcast();
		return;
	}
	// Create a HTTP DELETE request for the current user's channel
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("PATCH"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Chat->ConnectedUserCreds.OAuthToken));
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/channel_points/custom_rewards/redemptions?broadcaster_id=%s&reward_id=%s&id=%s"), *(Chat->ConnectedUserCreds.UserId), *RewardID, *RedemptionID));

	// Set the updated status in the body
	FString NewRedemptionStatusString;
	if (NewStatus == ETwitchRedemptionStatus::RS_UNFULFILLED)
	{
		NewRedemptionStatusString = TEXT("UNFULFILLED");
	}
	else if (NewStatus == ETwitchRedemptionStatus::RS_FULFILLED)
	{
		NewRedemptionStatusString = TEXT("FULFILLED");
	}
	else
	{
		// Assume cancelled by default in case of an unsupported type.
		NewRedemptionStatusString = TEXT("CANCELED");
	}

	// Create the JSON body
	TSharedPtr<FJsonObject> JSONObject = MakeShareable(new FJsonObject);
	JSONObject->SetStringField(TEXT("status"), NewRedemptionStatusString);

	// Serialize and set the request body to this newly created JSON object
	FString RequestBody;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JSONObject.ToSharedRef(), JsonWriter);
	HttpRequest->SetContentAsString(RequestBody);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UUpdateRedemptionStatus::HandleHttpResponse);
	HttpRequest->ProcessRequest();
}

void UUpdateRedemptionStatus::HandleHttpResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error to help debugging!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to execute points redemption update request!"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\nResponse Body: %s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Execute the failure delegate with default parameters
		OnFail.Broadcast();
		return;
	}

	// If so, we succeeded - we don't really care about the data in the response (which can be obtained by getting the reward redemptions anyway) so
	// succeed the request
	OnSuccess.Broadcast();
	return;
}

