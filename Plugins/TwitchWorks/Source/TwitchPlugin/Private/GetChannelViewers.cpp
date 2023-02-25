// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetChannelViewers.h"
#include "TwitchPlugin.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"


UGetChannelViewers::UGetChannelViewers(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
UGetChannelViewers *UGetChannelViewers::GetTwitchChannelViewers(FString ChannelName)
{
	UGetChannelViewers *HTTPTask = NewObject<UGetChannelViewers>();

	// Ensure a channel name was provided
	if (ChannelName.IsEmpty())
	{
		HTTPTask->OnFail.Broadcast(TArray<FString>());
		UE_LOG(LogTwitchWorks, Warning, TEXT("Empty channel name provided! Won't be able to get the channel's viewers!"));
		return HTTPTask;
	}

	// Convert the channel's name to lower case because Twitch expects it to be lower case
	HTTPTask->CreateHTTPRequest(ChannelName.ToLower());
	return HTTPTask;
}

void UGetChannelViewers::CreateHTTPRequest(FString ChannelName)
{
	// Create an HTTP request for the user lookup (this is a TMI endpoint -> no auth/kraken stuff required)
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	// Get the chatters (-> people connected to the chat / closest available way to get usernames of all viewers)
	HttpRequest->SetURL(FString::Printf(TEXT("http://tmi.twitch.tv/group/user/%s/chatters"), *ChannelName));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetChannelViewers::HandleHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UGetChannelViewers::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Ensure the request completed successfully
	if (!bSucceeded || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to get that channel's viewers!"));

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnFail.Broadcast(TArray<FString>());
		});
		return;
	}

	// Variable to store the channels' viewer's usernames in.
	TArray<FString> Viewers;

	// Turn the HTTP response's json string into a JSON object.
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		// Get all of the entries in the chatters objects (will include viewrs/admins/staff/all of that -> we don't really care about what they are, we just want to fetch them all)
		TMap<FString, TSharedPtr<FJsonValue>> Chatters = JSONObject->GetObjectField(TEXT("chatters"))->Values;
		// Outer loop - iterates over the various "types" of chatters
		for (auto &Entry : Chatters)
		{
			// Inner loop - iterates over all of the usernames in the given group (and built the array of all usernames based on this
			for (auto &jUsername : Entry.Value->AsArray())
			{
				Viewers.Add(jUsername->AsString());
			}
		}
	}

	// Successfully retrieved all chatter usernames/viewers - execute success!
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnSuccess.Broadcast(Viewers);
	});
	return;
}
