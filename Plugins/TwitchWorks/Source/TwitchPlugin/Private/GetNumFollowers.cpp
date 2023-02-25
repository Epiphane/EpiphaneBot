// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetNumFollowers.h"
#include "TwitchPlugin.h"
#include "HttpModule.h"
#include "Json.h"
#include "Interfaces/IHttpResponse.h"


UGetNumFollowers::UGetNumFollowers(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
UGetNumFollowers *UGetNumFollowers::GetTwitchNumFollowers(UTwitchChatConnector *_Chat, FTwitchUser _User)
{
	UGetNumFollowers* HTTPTask = NewObject<UGetNumFollowers>();
	HTTPTask->Chat = _Chat;
	HTTPTask->User = _User;
	return HTTPTask;
}

void UGetNumFollowers::Activate()
{

	if (!Chat || Chat->ConnectedUserCreds.OAuthToken.IsEmpty())
	{
		OnFail.Broadcast(-1);
		UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid Chat Credentials!"));
		return;
	}
	else if (User.UserId.IsEmpty())
	{
		OnFail.Broadcast(-1);
		UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid User ID!"));
		return;
	}

	CreateHTTPRequest(Chat->ConnectedUserCreds.OAuthToken, User.UserId);
}

void UGetNumFollowers::CreateHTTPRequest(FString OAuthToken, FString UserId)
{
	// Create an HTTP request for the user lookup
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *OAuthToken));

	// We need to try and get information on the streams currently running for the user.
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/users/follows?to_id=%s"), *UserId));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetNumFollowers::HandleHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UGetNumFollowers::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!bSucceeded || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		OnFail.Broadcast(-1);
		return;
	}
	else
	{
		// Successfully returned - transform into JSON!
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
		{
			// We're currently only interested in the total # of users -> the "total" attribute in the root of the response
			// (see: https://dev.twitch.tv/docs/api/reference#get-users-follows)

			int NumFollowers = -1;
			if (JSONObject->TryGetNumberField(TEXT("total"), NumFollowers))
			{
				OnSuccess.Broadcast(NumFollowers);
				return;
			}
			else
			{
				// Something went wrong extracting data!
				UE_LOG(LogTwitchWorks, Warning, TEXT("total not present in Get Followers API response!"))
				OnFail.Broadcast(-1);
				return;
			}
		}
	}

	UE_LOG(LogTwitchWorks, Warning, TEXT("RC: %i, RB: %s"),HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString())

	// If we got here, an error / unhandled response code occurred - something must have gone (horribly) wrong!
	OnFail.Broadcast(-1);
}