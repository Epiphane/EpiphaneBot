// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "RefreshTokenTask.h"
#include "HttpModule.h"
#include "Json.h"
#include "TwitchLoginTask.h"
#include "Interfaces/IHttpResponse.h"
#include "TwitchPlugin.h"


URefreshToken::URefreshToken(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
URefreshToken *URefreshToken::RefreshCredentials(FTwitchLoginCredentials _Credentials)
{
	URefreshToken *HTTPTask = NewObject<URefreshToken>();

	if (_Credentials.RefreshToken.IsEmpty())
	{
		UE_LOG(LogTwitchWorks, Warning, TEXT("Your refresh token is empty!"));
	}

	HTTPTask->Credentials = _Credentials;
	HTTPTask->CreateHTTPRequest(_Credentials.RefreshToken);
	return HTTPTask;
}

void URefreshToken::CreateHTTPRequest(FString RefreshToken)
{
	// Create an HTTP request for the token refreshment
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	// Ask the backend to refresh the passed in token.
	HttpRequest->SetURL(FString::Printf(TEXT("%s?action=REFRESH&refresh_token=%s"), *GetMutableDefault<UTwitchRuntimeSettings>()->twitchLoginApiURL, *RefreshToken));
	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &URefreshToken::HandleHTTPRequest);
	HttpRequest->ProcessRequest();
}

void URefreshToken::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// The backend will return a non-OK code for failure. This will be caught by this line.
	if (!bSucceeded || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		OnFail.Broadcast(Credentials);
		return;
	}

	// Turn the response into a JSON object so it's easier to parse
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		// Has the user successfully completed oauth?
		if (JSONObject->HasField("success") && JSONObject->GetBoolField(TEXT("success")))
		{
			// Attempt to extract the new token, and update our copy of the credentials to reflect it.
			FString Token = JSONObject->HasField("token") ? JSONObject->GetStringField(TEXT("token")) : TEXT("");
			Credentials.OAuthToken = Token;

			OnRefreshed.Broadcast(Credentials);
			return;
		}
	}

	// If we got here, an error / unhandled response code occured - something must have gone (horribly) wrong!
	OnFail.Broadcast(Credentials);
	return;
}
