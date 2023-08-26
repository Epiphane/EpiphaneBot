#include "TwitchGetChatters.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "TwitchPubSubConnection.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "TwitchRuntimeSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogTwitchGetChatters, Log, All);

UTwitchGetChatters::UTwitchGetChatters(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UTwitchGetChatters* UTwitchGetChatters::GetChatters(const UObject* worldContextObject)
{
	UTwitchEventsSubsystem* Subsystem = worldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UTwitchEventsSubsystem>();
	if (!ensure(Subsystem))
	{
		UE_LOG(LogTwitchGetChatters, Error, TEXT("Could not find UTwitchEventsSubsystem"));
		return nullptr;
	}

	UTwitchGetChatters* Task = NewObject<UTwitchGetChatters>();
	Task->MakeRequest(Subsystem->GetCredentials());
	return Task;
}

void UTwitchGetChatters::MakeRequest(const FTwitchLoginCredentials& Credentials)
{
	// Create an HTTP request for the user lookup
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Credentials.OAuthToken));
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/chat/chatters?broadcaster_id=%s&moderator_id=%s"), *Credentials.UserId, *Credentials.UserId));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTwitchGetChatters::HandleHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UTwitchGetChatters::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Ensure the request completed successfully
	if (!bSucceeded || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		UE_LOG(LogTwitchGetChatters, Warning, TEXT("Failed to get that channel's viewers!"));

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnFail.Broadcast(TArray<FChatter>());
		});
		return;
	}

	// Variable to store the channels' viewer's usernames in.
	TArray<FChatter> Chatters;

	// Turn the HTTP response's json string into a JSON object.
	FGetChattersResponse Response;
	if (FJsonObjectConverter::JsonObjectStringToUStruct(HttpResponse->GetContentAsString(), &Response))
	{
		// Outer loop - iterates over the various "types" of chatters
		for (auto& Entry : Response.data)
		{
			Chatters.Add(FChatter{ FCString::Strtoi64(*Entry.user_id, nullptr, 10), Entry.user_name });
		}
	}

	// Successfully retrieved all chatter usernames/viewers - execute success!
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnSuccess.Broadcast(Chatters);
	});
}
