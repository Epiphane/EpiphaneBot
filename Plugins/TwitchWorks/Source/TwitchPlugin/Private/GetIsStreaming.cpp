// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetIsStreaming.h"
#include "TwitchPlugin.h"
#include "HttpModule.h"
#include "Json.h"
#include "Interfaces/IHttpResponse.h"


UGetIsStreaming::UGetIsStreaming(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
UGetIsStreaming *UGetIsStreaming::GetTwitchIsStreaming(UTwitchChatConnector *Chat, FTwitchUser User)
{
	UGetIsStreaming *HTTPTask = NewObject<UGetIsStreaming>();

	if (!Chat || Chat->ConnectedUserCreds.OAuthToken.IsEmpty())
	{
		HTTPTask->OnFail.Broadcast(TEXT(""), -1);
		UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid Chat Credentials!"));
		return HTTPTask;
	}
	else if (User.UserId.IsEmpty()) 
	{
		HTTPTask->OnFail.Broadcast(TEXT(""), -1);
		UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid User ID!"));
		return HTTPTask;
	}

	HTTPTask->CreateHTTPRequest(Chat->ConnectedUserCreds.OAuthToken, User.UserId);
	return HTTPTask;
}

void UGetIsStreaming::CreateHTTPRequest(FString OAuthToken, FString UserId)
{
	// Create an HTTP request for the user lookup
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *OAuthToken));

	// We need to try and get information on the streams currently running for the user.
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/streams?user_id=%s"), *UserId));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetIsStreaming::HandleHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UGetIsStreaming::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Based on https://dev.twitch.tv/docs/api/reference#get-streams -> if data has information, there is a stream
	if (!bSucceeded)
	{
		OnFail.Broadcast(TEXT(""), -1);
		return;
	}
	else if (HttpResponse->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		// Successfully returned - the stream "must" exist!
		// Attempt to turn the load the response's json
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
		{
			// Deal with the fact twitch supports getting multiple users at once (we're only getting one)
			TArray<TSharedPtr<FJsonValue>> DataValues = JSONObject->GetArrayField(TEXT("data"));

			// Assume the user is streaming if there is data about the streaming available
			if (DataValues.Num() > 0 && DataValues[0])
			{
				// Attempt to extract additional viewership/metadata about the straem
				int NumViewers = -1;
				FString StreamTitle = TEXT("");

				TSharedPtr<FJsonObject> StreamData = DataValues[0]->AsObject();
				if (StreamData)
				{
					NumViewers = StreamData->GetNumberField(TEXT("viewer_count"));
					StreamTitle = StreamData->GetStringField(TEXT("title"));
				}
				//StreamData.
				OnStreaming.Broadcast(StreamTitle, NumViewers);
				return;
			}
			else
			{
				// Seeing as no data about the stream is available, there likely isn't a stream to speak of
				OnNotStreaming.Broadcast(TEXT(""), -1);
				return;
			}
		}
	}
	else if (HttpResponse->GetResponseCode() == EHttpResponseCodes::NotFound)
	{
		// It wasn't found, so assume the user is not streaming.
		OnNotStreaming.Broadcast(TEXT(""), -1);
		return;
	}

	UE_LOG(LogTwitchWorks, Warning, TEXT("RC: %i, RB: %s"),HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString())

	// If we got here, an error / unhandled response code occurred - something must have gone (horribly) wrong!
	OnFail.Broadcast(TEXT(""), -1);
}