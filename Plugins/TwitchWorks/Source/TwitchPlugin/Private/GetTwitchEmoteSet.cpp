// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetTwitchEmoteSet.h"
#include "TwitchPlugin.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Json.h"
#include "TwitchChatConnector.h"
#include "TwitchRuntimeSettings.h"

UGetTwitchEmoteSet::UGetTwitchEmoteSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGetTwitchEmoteSet *UGetTwitchEmoteSet::GetTwitchEmoteSet(UTwitchChatConnector* _Chat, int32 _EmoteSet)
{
	UGetTwitchEmoteSet *Task = NewObject<UGetTwitchEmoteSet>();
	Task->Chat = _Chat;
	Task->EmoteSet = _EmoteSet;
	return Task;
}

void UGetTwitchEmoteSet::Activate()
{
	if (!Chat || Chat->ConnectedUserCreds.OAuthToken.IsEmpty())
	{
		if (FTwitchPluginModule::LastOAuthToken.IsEmpty())
		{
			OnFail.Broadcast(TArray<FTwitchEmoticon>());
			UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid Chat Credentials passed to GetTwitchEmoteSet! Please note that credentials (passed through the chat) are now required for all Helix API calls (the old Kraken API has unfortunately been depricated and will soon be removed)!"));
			return;
		}
		else
		{
			// No chat passed in, but we do have a previous successful login. Attempt to use this OAuth token for the request!
			// TODO: Remove, this is temporary to preserve backwards compatibility until users can change their code
			UE_LOG(LogTwitchWorks, Warning, TEXT("Deprication Notice: GetTwitchEmoteSet now required OAuth credentials to call the Twitch API, meaning you must pass in the chat. This appears to not currently be the case, but we detected that you did previously log in successfully. Using these credentials for now, but please note that this will be removged in the future!"))
			CreateHTTPRequest(EmoteSet, FTwitchPluginModule::LastOAuthToken);
			return;
		}
	}
	CreateHTTPRequest(EmoteSet, Chat->ConnectedUserCreds.OAuthToken);
	return;
}

void UGetTwitchEmoteSet::CreateHTTPRequest(int32 _EmoteSet, FString OAuthToken)
{
	// Create an HTTP request to get the emotes in an emote set. (NOTE doens't actually return the images)
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/chat/emotes/set?emote_set_id=%i"), EmoteSet));
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *OAuthToken));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetTwitchEmoteSet::HandleHTTPRequest);
	
	// Execute the newly created emote fetching HTTP request
	HttpRequest->ProcessRequest();

}

void UGetTwitchEmoteSet::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!bSucceeded)
	{
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to get twitch emote set!"));

		// Ensure the callback executes on the main game thread to avoid blueprint threading issues.
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnFail.Broadcast(TArray<FTwitchEmoticon>());
		});
	}

	// Variable to store the emoticons in.
	TArray<FTwitchEmoticon> Emoticons;

	// Turn the HTTP response's json string into a JSON object.
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		// Iterate over the the requested emoticon set's emoticon json
		TArray<TSharedPtr<FJsonValue>> EmoticonsJSONValues = JSONObject->GetArrayField(TEXT("data"));
		for (TSharedPtr<FJsonValue> EmoticonJSONValue : EmoticonsJSONValues)
		{
			// Extract the emoticon code/id out of the json
			TSharedPtr<FJsonObject> EmoticonObject = EmoticonJSONValue->AsObject();
			FTwitchEmoticon Emoticon;
			Emoticon.EmoteSet = EmoteSet;
			Emoticon.Code = EmoticonObject->GetStringField(TEXT("name"));
			Emoticon.Id = EmoticonObject->GetStringField(TEXT("id"));

			// Add it to the array of emoticons.
			Emoticons.Add(Emoticon);
		}
	}

	// Ensure the callback executes on the main game thread to avoid issues with breakpoints not working etc.
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnSuccess.Broadcast(Emoticons);
	});
	
}
