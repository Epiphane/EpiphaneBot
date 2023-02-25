// Copyright 2017-2022 HowToCompute. All Rights Reserved.


#include "LocalTwitchLogin.h"

#include "Engine/Engine.h"
#include "TimerManager.h"

#include "IHttpRouter.h"
#include "HttpRequestHandler.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"

#include "Json.h"

#include "TwitchPlugin.h"
#include "TwitchRuntimeSettings.h"
#include "HTTPServer/Private/HttpListener.h"


// Twitch chat connector constructior
ULocalTwitchLoginTask::ULocalTwitchLoginTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

ULocalTwitchLoginTask* ULocalTwitchLoginTask::PerformLocalTwitchLogin(UObject* _WorldContextObject)
{
	ULocalTwitchLoginTask* LoginTask = NewObject<ULocalTwitchLoginTask>();
	LoginTask->StartLoginRequest(_WorldContextObject);
	return LoginTask;
}


void ULocalTwitchLoginTask::BeginDestroy()
{
	Super::BeginDestroy();

	PerformCleanup();
}

void ULocalTwitchLoginTask::StartLoginRequest(UObject* _WorldContextObject)
{
	// Set the class' world context object to the passed in world context object (this way we can access it in all of our functions)
	WorldContextObject = _WorldContextObject;

	// UE4's HTTP Server module is often not used, so we need to make sure the module is loaded.
	// NOTE: Not doing this during plugin start as the local login is experimental and not used by the majority of users -> only load it when needed to avoid unnecissary modules being loaded.
	if (!FModuleManager::Get().IsModuleLoaded("HttpServer"))
	{
		FModuleManager::Get().LoadModule("HttpServer");
	}


	HttpRouter = FHttpServerModule::Get().GetHttpRouter(GetMutableDefault<UTwitchRuntimeSettings>()->localLoginPort);

	FHttpRequestHandler OnTwitchCallback = [&](const FHttpServerRequest& Req, const FHttpResultCallback& CB) -> bool {
		UE_LOG(LogTwitchWorks, Log, TEXT("User has requested the Twitch auth callback page!"));

		// Return the (hard-coded) callback page which contains a basic UI and some JS code to extract the token from the hash and send it to the token submission endpoint.
		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(
			TEXT("<!DOCTYPE html>\r\n<html lang=\"en\">\r\n\t<head>\r\n\t\t<meta charset=\"utf-8\">\r\n\t\t<title>TwitchWorks Local Login</title>\r\n\t\t<script>\r\n\t\t\t// Function based on https://stackoverflow.com/a/16046607, but modified to parse the hash instead of the query string\r\n\t\t\tfunction getQuerystringNameValue(name)\r\n\t\t\t{\r\n\r\n\t\t\t\tvar winURL = window.location.hash;\r\n\t\t\t\tvar queryStringArray = winURL.split(\"#\");\r\n\t\t\t\tvar queryStringParamArray = queryStringArray[1].split(\"&\");\r\n\t\t\t\tvar nameValue = null;\r\n\r\n\t\t\t\tfor ( var i=0; i<queryStringParamArray.length; i++ )\r\n\t\t\t\t{           \r\n\t\t\t\t\tqueryStringNameValueArray = queryStringParamArray[i].split(\"=\");\r\n\r\n\t\t\t\t\tif ( name == queryStringNameValueArray[0] )\r\n\t\t\t\t\t{\r\n\t\t\t\t\t\tnameValue = queryStringNameValueArray[1];\r\n\t\t\t\t\t}                       \r\n\t\t\t\t}\r\n\r\n\t\t\t\treturn nameValue;\r\n\t\t\t}\r\n\t\t\t\r\n\t\t\t// Helper function to centralize status message updating; no need for a fancy framework or even \r\n\t\t\tfunction updateStatus (new_status)\r\n\t\t\t{\r\n\t\t\t\tdocument.getElementById(\"current_status\").innerHTML = new_status;\r\n\t\t\t}\r\n\t\t\t\r\n\t\t\t// This function will parse the credentials from the hash, ensure they exist/are probably valid, and finally submit them to the local UE4 web server for final processing\r\n\t\t\tfunction credential_handler ()\r\n\t\t\t{\r\n\t\t\t\t// Ensure we got a hash in our URL\r\n\t\t\t\tif (!window.location.hash)\r\n\t\t\t\t{\r\n\t\t\t\t\tconsole.log(\"No hash available\");\r\n\t\t\t\t\talert(\"Twitch login failure! Please try again! (NO_HASH)\");\r\n\t\t\t\t\tupdateStatus(\"NO_HASH\");\r\n\t\t\t\t\treturn;\r\n\t\t\t\t}\r\n\r\n\t\t\t\t// Parse the relevant parameters from the hash\r\n\t\t\t\tvar access_token = getQuerystringNameValue(\"access_token\");\r\n\t\t\t\tvar token_type = getQuerystringNameValue(\"token_type\");\r\n\r\n\t\t\t\t// Ensure all required parameters are present\r\n\t\t\t\tif (!access_token || !token_type)\r\n\t\t\t\t{\r\n\t\t\t\t\tconsole.log(\"Access token or token type missing from hash!\");\r\n\t\t\t\t\talert(\"Twitch login failure! Please try again! (ACCESS_TOKEN_OR_TYPE_MISSING)\");\r\n\t\t\t\t\tupdateStatus(\"ACCESS_TOKEN_OR_TYPE_MISSING\");\r\n\t\t\t\t\treturn;\r\n\t\t\t\t}\r\n\r\n\t\t\t\t// Ensure we received a bearer token\r\n\t\t\t\tif (token_type !== \"bearer\")\r\n\t\t\t\t{\r\n\t\t\t\t\tconsole.log(\"Expected bearer token, but instead got: \"+token_type);\r\n\t\t\t\t\talert(\"Twitch login failure! Please try again! (INVALID_TOKEN_TYPE)\");\r\n\t\t\t\t\tupdateStatus(\"INVALID_TOKEN_TYPE\");\r\n\t\t\t\t\treturn;\r\n\t\t\t\t}\r\n\r\n\t\t\t\t// Send the relevant information off to our UE4 to complete the login (UE4 won\'t receive the hash we received, hence this whole parsing & sending thing).\r\n\t\t\t\tvar xhttp = new XMLHttpRequest();\r\n\t\t\t\txhttp.open(\"POST\", \"submit_token\", true);\r\n\t\t\t\txhttp.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\r\n\t\t\t\t\r\n\t\t\t\t// Bind the sync success/failure handlers\r\n\t\t\t\txhttp.onload = function (e) {\r\n\t\t\t\t\tif (xhttp.readyState === 4) {\r\n\t\t\t\t\t\tif (xhttp.status === 200) {\r\n\t\t\t\t\t\t\tconsole.log(\"Successfully submitted token!\");\r\n\t\t\t\t\t\t\tupdateStatus(\"Done! Please switch back to your application and start playing\");window.close();\r\n\t\t\t\t\t\t} else {\r\n\t\t\t\t\t\t\tconsole.error(xhttp.statusText);\r\n\t\t\t\t\t\t\tconsole.log(\"Unable to communicate with game! Please take a look at the console for more information\");\r\n\t\t\t\t\t\t}\r\n\t\t\t\t\t}\r\n\t\t\t\t};\r\n\t\t\t\txhttp.onerror = function (e) {\r\n\t\t\t\t  console.error(xhttp.statusText);\r\n\t\t\t\t  console.log(\"Unable to communicate with game! Please take a look at the console for more information\");\r\n\t\t\t\t};\r\n\t\t\t\t\r\n\t\t\t\txhttp.send(\"access_token=\"+access_token);\r\n\t\t\t\tupdateStatus(\"waiting for game to accept access token\");\r\n\t\t\t\treturn;\r\n\t\t\t}\r\n\t\t\t\r\n\t\t\t// Call the Twitch credential handling code when the page finishes loading\r\n\t\t\twindow.addEventListener(\'DOMContentLoaded\', (event) => {\r\n\t\t\t\tconsole.log(\'DOM loaded; going to start TwitchWorks code\');\r\n\t\t\t\tupdateStatus(\"Processing...\");\r\n\t\t\t\tcredential_handler();\r\n\t\t\t});\r\n\t\t</script\r\n\t</head>\r\n\r\n\t<body> \r\n\t\t<h1>TwitchWorks Local Login</h1>\r\n\t\t<h3>Status: <span id=\"current_status\">initializing</span>...</h3>\r\n\t\t<noscript>Your browser does not support JavaScript! You\'ll need to enable JavaScript to use TwitchWorks Local Login</noscript>\r\n\t</body>\r\n</html>"),
			TEXT("text/html")
		);
		CB(MoveTemp(Response));
		return true;
	};

	FHttpRequestHandler OnTokenReceived = [&](const FHttpServerRequest& Req, const FHttpResultCallback& CB) -> bool {
		UE_LOG(LogTwitchWorks, Log, TEXT("Received Token Submission Attempt!"));

		// NOTE: UE4's HTTP server unfortunately doesn't appear to have any built-in utilities to get the body as a string or to parse it for POST parameters (only query/path, which both don't contain anything from the body, or at least, they didn't in my testing)
		// Our workaround for this is a quick hack to parse out the access token assuming it follows the "standard" form encoding stuff, doesn't contain any weird (escaped) characters, and is the only parameter in the boyd.
		// This should work for now, but might want to be revisted with an improved parser in the future.
		// NOTE2: Body should be something like the following: "access_token=XYZ"
		// NOTE3: Testing in packaged games revealed that the TArray doesn't (always) create a neat null-terminater, so to ensure we don't suffer a buffer overflow, we'll always add one as the last element
		TArray<uint8> BodyBytes = Req.Body;
		BodyBytes.Add((uint8)'\0');
		FString Body = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(BodyBytes.GetData())));

		// Remove the key (the only expected string other than the token value; NOTE: this is why the assumptions made above are required) and store it in the global/class scope
		Body.RemoveFromStart(TEXT("access_token="));
		AccessToken = Body;

		// Unfortunately, Twitch does not provide us with the user's login/username. Execute an API request to obtain it, as we will require it to connect over IRC.
		StartUsernameRequest();

		// Return a very basic success message to avoid the built-in 404 page causing issues.
		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(TEXT("{\"success\": true}"), TEXT("application/json"));
		CB(MoveTemp(Response));
		return true;
	};

	// The listener that fires when the user returns from Twitch
	// NOTE: For some or other reason, UE4's built-in web server will issue a check() failure if the path is a root, so we must use a non-root path
	CallbackRouteHandle = HttpRouter->BindRoute(FHttpPath(ListenPath), EHttpServerRequestVerbs::VERB_GET, OnTwitchCallback);

	// Listener that fires when the user has successfully authenticated and the JS code has parsed/sent the access token to us (completes login)
	TokenSubmissionRouteHandle = HttpRouter->BindRoute(FHttpPath(SubmitTokenPath), EHttpServerRequestVerbs::VERB_POST, OnTokenReceived);

	FHttpServerModule::Get().StartAllListeners();

	// Generate the URL of the listener (intermediary step as we might want to use it in other places/update this code more easily)
	FString CallbackURL = FString::Printf(TEXT("http://localhost:%d%s"), GetMutableDefault<UTwitchRuntimeSettings>()->localLoginPort, *ListenPath);

	// Start a tiemout checker to ensure we don't get stuck forever
	if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().SetTimer(TimeoutTimerHandle, this, &ULocalTwitchLoginTask::HandleTimeout, GetMutableDefault<UTwitchRuntimeSettings>()->localLoginTimeout, false);
	
	// Generate the OAuth scope string / +separated list of OAuth Helix scopes
	FString OAuthScopeString = FString::Join(GetMutableDefault<UTwitchRuntimeSettings>()->localLoginScopes, TEXT("+"));

	// Open the user's browser and send them to Twitch to complete the OAuth request
	FPlatformProcess::LaunchURL(*FString::Printf(TEXT("https://id.twitch.tv/oauth2/authorize?response_type=token&client_id=%s&redirect_uri=%s&scope=%s"), *GetMutableDefault<UTwitchRuntimeSettings>()->clientID, *CallbackURL, *OAuthScopeString), TEXT(""), nullptr);
}

void ULocalTwitchLoginTask::StartUsernameRequest()
{
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));

	// The users endpoint without any additional parameters should give us information about the currently authenticated user
	HttpRequest->SetURL(TEXT("https://api.twitch.tv/helix/users"));

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULocalTwitchLoginTask::HandleUsernameHTTPRequest);

	HttpRequest->ProcessRequest();
}

void ULocalTwitchLoginTask::HandleUsernameHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the request successful?
	if (bSucceeded && HttpResponse->GetContentAsString().Len() > 0 && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Turn the response into a JSON object so it's easier to parse
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(JSONReader, JSONObject) && JSONObject)
		{
			// Is the username (or technically the "login" name) present?
			TArray<TSharedPtr<FJsonValue>> jDataArray = JSONObject->GetArrayField(TEXT("data"));
			if (jDataArray.Num() == 1)
			{
				TSharedPtr<FJsonObject> jData = jDataArray[0]->AsObject();
				if (jData && jData->HasTypedField<EJson::String>("login"))
				{
					// Extract the relevant data and succeed the request
					FString Username = jData->GetStringField(TEXT("login"));
					FString UserId = jData->GetStringField(TEXT("id"));

					FTwitchLoginCredentials Credentials;
					Credentials.Username = Username;
					Credentials.UserId = UserId;
					Credentials.OAuthToken = AccessToken;
					Credentials.RefreshToken = TEXT("");

					// Make sure the GetEmoteSet node doesn't break
					// TODO: Remove - this is a temporary thing
					FTwitchPluginModule::LastOAuthToken = AccessToken;

					// Do the OnSuccess callback in the game thread to avoid issues with threading & blueprints (like breakpoints not working, etc.)
					AsyncTask(ENamedThreads::GameThread, [=]()
						{
							OnSuccess.Broadcast(Credentials);
						});
					PerformCleanup();
					return;
				}
			}

			// Fallthrough that catches all (scoped) paths that don't end in the success stuff being broadcasted/the fn returning
			UE_LOG(LogTwitchWorks, Error, TEXT("Unable to retrieve username from Twitch API: do you have the right scopes?."));
		}
	}
	else
	{
		UE_LOG(LogTwitchWorks, Error, TEXT("Unable to obtain username after local login!!"));
	}

	// "Blanket" error handlers -> above error places simply print a message and fall through to this
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnFail.Broadcast(FTwitchLoginCredentials());
	});
	PerformCleanup();
}

void ULocalTwitchLoginTask::HandleTimeout()
{
	UE_LOG(LogTwitchWorks, Warning, TEXT("Timeout threshold exceeded for local TwitchWorks login - failing request. (TIMEOUT: %f[s])"), GetMutableDefault<UTwitchRuntimeSettings>()->localLoginTimeout);
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		OnFail.Broadcast(FTwitchLoginCredentials());
	});

	PerformCleanup();
}

void ULocalTwitchLoginTask::PerformCleanup()
{
	// Ensure this isn't a duplicate cleanup
	if (bCleanupDone)
	{
		UE_LOG(LogTwitchWorks, Verbose, TEXT("Local TwitchWorks login cleanup already performed. Stopping redundant attempt!"));
		return;
	}

	// As of writing, UE4 does not appear to support stopping a single listener, but only stopping all listeners.
	// Since stopping all listeners may cause interference with other code, we'll instead simply unbind from the routes and call it a day.
	if (HttpRouter.IsValid())
	{
		if (CallbackRouteHandle.IsValid())
		{
			HttpRouter->UnbindRoute(CallbackRouteHandle);
		}

		if (TokenSubmissionRouteHandle.IsValid())
		{
			HttpRouter->UnbindRoute(TokenSubmissionRouteHandle);
		}
	}

	// Kill the listener too if it has been configured as such (to avoid interference between different instances of the engine and it not auto-unbinding)
	// NOTE: As of writing, there is no (public) way to get the Listeners object / to shut down a single port, so we need to kill all listeners
	// NOTE: If other plugins use same port, this could interfere hence option to disable it
	if (GetMutableDefault<UTwitchRuntimeSettings>()->bStopAllListenersOnLocalLoginComplete)
	{
		FHttpServerModule::Get().StopAllListeners();
		//auto ListenerResult = FHttpServerModule::Get().Listeners.Find(GetMutableDefault<UTwitchRuntimeSettings>()->localLoginPort);
		//if (ListenerResult && *ListenerResult)
		//{
		//	(*ListenerResult)->StopListening();
		//}
	}

	// Clear all the remaining active timers timers
	if (IsValid(WorldContextObject) && GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)->GetTimerManager().ClearAllTimersForObject(this);

	bCleanupDone = true;
}
