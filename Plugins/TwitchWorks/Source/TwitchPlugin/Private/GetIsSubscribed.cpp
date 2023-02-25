// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetIsSubscribed.h"
#include "TwitchPlugin.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "TwitchPlugin.h"


UGetIsSubscribed::UGetIsSubscribed(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
UGetIsSubscribed *UGetIsSubscribed::GetTwitchIsSubscribed(UTwitchChatConnector *Chat, FTwitchUser User, FString ChannelId)
{
	UGetIsSubscribed *HTTPTask = NewObject<UGetIsSubscribed>();

	if (!Chat || Chat->ConnectedUserCreds.OAuthToken.IsEmpty()) { HTTPTask->OnFail.Broadcast(); UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid Chat Credentials!")); return HTTPTask; }
	if (User.UserId.IsEmpty() || ChannelId.IsEmpty()) { HTTPTask->OnFail.Broadcast(); UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid User or Channel ID!")); return HTTPTask; }

	HTTPTask->CreateHTTPRequest(Chat->ConnectedUserCreds.OAuthToken, User.UserId, ChannelId);
	return HTTPTask;
}

void UGetIsSubscribed::CreateHTTPRequest(FString OAuthToken, FString UserId, FString ChannelId)
{
	// Create an HTTP request for the user lookup
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *OAuthToken));

	// We need to see if the subscription object exists for this user & channel.
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/subscriptions/user?broadcaster_id=%s&user_id=%s"), *ChannelId, *UserId));
	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetIsSubscribed::HandleHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UGetIsSubscribed::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Based on https://dev.twitch.tv/docs/api/reference#check-user-subscription 200 -> the subscription exists, 404 -> the sub doesn't exist, other -> some error happened!
	if (!bSucceeded)
	{
		// Something went seriously wrong when handling the HTTP request
	}
	else if (HttpResponse->GetResponseCode() == EHttpResponseCodes::Ok)
	{
		// Successfully returned - the subscription "must" exist!
		OnSubscribed.Broadcast();
		return;
	}
	else if (HttpResponse->GetResponseCode() == EHttpResponseCodes::NotFound)
	{
		// It wasn't found, so assume the subscription to that channel doesn't exist for that user.
		OnNotSubscribed.Broadcast();
		return;
	}

	UE_LOG(LogTwitchWorks, Warning, TEXT("RC: %i, RB: %s"),HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString())

	// If we got here, an error / unhandled response code occured - something must have gone (horribly) wrong!
	OnFail.Broadcast();
}
