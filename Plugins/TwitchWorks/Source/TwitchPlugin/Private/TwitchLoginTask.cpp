// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "TwitchLoginTask.h"
#include "TwitchPlugin.h"
#include "TwitchRuntimeSettings.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Engine.h"
#include "Json.h"
#include "Async/Async.h"

// #if WITH_EDITOR
// #include "Editor.h"
// #endif

UTwitchLoginTask::UTwitchLoginTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), WorldContextObject(nullptr)
{
}

UTwitchLoginTask *UTwitchLoginTask::PerformTwitchLogin(UObject* _WorldContextObject)
{
	UTwitchLoginTask *LoginTask = NewObject<UTwitchLoginTask>();
	LoginTask->WorldContextObject = _WorldContextObject;
	return LoginTask;
}

void UTwitchLoginTask::BeginDestroy()
{
	Super::BeginDestroy();

	// Clear all the remaining active timers timers
	if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().ClearAllTimersForObject(this);
	}
}


void UTwitchLoginTask::Activate()
{
	// Ask the TwitchWorks API to start a login request
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	// Use the twitch game api endpoint set in the settings module
	HttpRequest->SetURL(FString::Printf(TEXT("%s?action=START"), *GetMutableDefault<UTwitchRuntimeSettings>()->twitchLoginApiURL));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTwitchLoginTask::HandleStartHTTPRequest);
	HttpRequest->ProcessRequest();

	// Start a (user-configurable) timeout "watchdog" that makes sure the entire process won't take too long
	// NOTE: Starting it here rather than after we send the user to the browser to avoid a long taking HTTP request or something in that part of the code causing a skewed timeout/this way the "entire" procedure is "measured"
	// NOTE: If the timeout is set to -1, no timeout is used
	if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) && GetMutableDefault<UTwitchRuntimeSettings>()->twitchLoginTimeout != -1)
	{
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().SetTimer(timeoutTimerHandle, this, &UTwitchLoginTask::OnTimeoutReached, GetMutableDefault<UTwitchRuntimeSettings>()->twitchLoginTimeout, false);
	}
}

void UTwitchLoginTask::DoTimerCallback()
{
	// Did the user focus back on the viewport/game? (saves polling the API when the user probably is still logging in)
	if (GEngine->GameViewport->IsFocused(GEngine->GameViewport->Viewport) || GEngine->GameViewport->Viewport->IsForegroundWindow())
	{
		// If so, start an API polling request to see whether or not the user has logged in and call this timer again in 5 seconds (so we don't overload the API)
		StartPollingRequest();
		if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) && focusCheckTimerHandle.IsValid() && !pollRequestTimerHandle.IsValid())
		{
			// It looks like the focus check timer is still active and the (new) poll start timer isn't - fix this
			// NOTE: Should only trigger once
			GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().ClearTimer(focusCheckTimerHandle);
			focusCheckTimerHandle.Invalidate();
			
			GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().SetTimer(pollRequestTimerHandle, this, &UTwitchLoginTask::DoTimerCallback, 5.f, true);
		}
	}

	// If not, do nothing and wait for the next timer invocation (in ~1s)
}

void UTwitchLoginTask::StartPollingRequest()
{
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

	// Set the URL to the setting's game login api endpoint, with the POLLing action and the earlier created key/token combo.
	HttpRequest->SetURL(FString::Printf(TEXT("%s?action=POLL&key=%s&token=%s"), *GetMutableDefault<UTwitchRuntimeSettings>()->twitchLoginApiURL, *requestKey, *requestToken));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTwitchLoginTask::HandlePollingHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UTwitchLoginTask::HandlePollingHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the request (probably) successful?
	if (bSucceeded && HttpResponse->GetContentAsString().Len() > 0)
	{
		// Turn the response into a JSON object so it's easier to parse
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
		{
			// Has the user successfully completed oauth?
			if (JSONObject->HasField("success") && JSONObject->GetBoolField(TEXT("success")))
			{
				// Attempt to extract the key/token
				FString Username = JSONObject->HasField("username") ? JSONObject->GetStringField(TEXT("username")) : TEXT("");
				FString Token = JSONObject->HasField("token") ? JSONObject->GetStringField(TEXT("token")) : TEXT("");
				FString RefreshToken = JSONObject->HasField("refresh_token") ? JSONObject->GetStringField(TEXT("refresh_token")) : TEXT("");
				FString UserId = JSONObject->HasField("twitch_id") ? JSONObject->GetStringField(TEXT("user_id")) : TEXT(""); // Added in TwiWorks 1.0.20 - requires backend update!

				// Copy the credentials into a TwtichLoginCredentials structs so it's nicer to handle in BP
				FTwitchLoginCredentials Credentials;
				Credentials.Username = Username;
				Credentials.OAuthToken = Token;
				Credentials.UserId = UserId;
				Credentials.RefreshToken = RefreshToken;

				// Make sure the GetEmoteSet node doesn't break
				// TODO: Remove - this is a temporary thing
				FTwitchPluginModule::LastOAuthToken = Token;

				// Do the OnSuccess callback in the game thread to avoid issues with threading & blueprints (like breakpoints not working, etc.)
				AsyncTask(ENamedThreads::GameThread, [=]()
				{
					OnSuccess.Broadcast(Credentials);
				});

				// Successfully logged in!
				// Cancel the timer for the next request
				if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
				{
					GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().ClearAllTimersForObject(this);
				}
				return;
			}
			else
			{
				UE_LOG(LogTwitchWorks, Log, TEXT("User hasn't completed twitch OAuth yet."));
			}
		}
	}
	else
	{
		UE_LOG(LogTwitchWorks, Warning, TEXT("Error completing User Login POLL-ing request!"));
	}
}

void UTwitchLoginTask::OnTimeoutReached()
{
	UE_LOG(LogTwitchWorks, Warning, TEXT("Twitch login procedure took too long to complete! Considering it failed!"));
	
	// Clear all the remaining active timers timers
	if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().ClearAllTimersForObject(this);
	}

	// Inform the BP about the login failure
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnTimeout.Broadcast(FTwitchLoginCredentials());
	});
}

void UTwitchLoginTask::HandleStartHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentAsString().Len() <= 0 )
	{
		UE_LOG(LogTwitchWorks, Log, TEXT("Error connecting to TwitchWorks API! Response Content String: %s (response code: %i)"), *HttpResponse->GetContentAsString(), HttpResponse->GetResponseCode());
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnFail.Broadcast(FTwitchLoginCredentials());
		});
		return;
	}

	// Turn the response into a JSON object so it's easier to parse
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		// Attempt to extract the key/token
		requestKey = JSONObject->GetStringField(TEXT("key"));
		requestToken = JSONObject->GetStringField(TEXT("token"));

		// Send the user to their browser (so it may seem more trusted & it can auto login if they are already logged in)
		FPlatformProcess::LaunchURL(*FString::Printf(TEXT("%s?key=%s&token=%s"), *GetMutableDefault<UTwitchRuntimeSettings>()->twitchLoginURL, *requestKey, *requestToken), TEXT(""), nullptr);

		// Start a timer that will check if the user has focused back on their game every second
		if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().SetTimer(focusCheckTimerHandle, this, &UTwitchLoginTask::DoTimerCallback, 1.f, true);
		}
		return;
	}

	// Something must have gone wrong, call the OnFail delegate in the gamethread to avoid threading issues
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnFail.Broadcast(FTwitchLoginCredentials());
	});
}
