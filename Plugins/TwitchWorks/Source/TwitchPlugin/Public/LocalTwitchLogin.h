// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HttpServerModule.h"
#include "HttpResultCallback.h"
#include "HttpServerRequest.h"
#include "HttpRouteHandle.h"

#include "Http.h"

#include "TwitchLoginTask.h"

#include "LocalTwitchLogin.generated.h"

/*
* Async local login task class.
*/
UCLASS()
class TWITCHPLUGIN_API ULocalTwitchLoginTask : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* EXPERIMENTAL - Performs Twitch login using a local server. 
	* We still highly recommend the use of the backend for production games, as this may very well hit firewall and other unexpected issues for end-users.
	*
	* @param WorldContextObject The world context object.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Perform Local Twitch Login - EXPERIMENTAL", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "TwitchWorks|EXPERIMENTAL")
	static ULocalTwitchLoginTask *PerformLocalTwitchLogin(UObject* WorldContextObject);

public:
	// Gets called when the user successfully logs in
	UPROPERTY(BlueprintAssignable)
	FTwitchLoginDelegate OnSuccess;

	// Gets called when there is an error logging in.
	UPROPERTY(BlueprintAssignable)
	FTwitchLoginDelegate OnFail;

public:
	// Stops the async task. Helps solve wierd behavior when the editor gets closed
	virtual void BeginDestroy() override;

public:
	// Starts the local web server (& callbacks) and sends the user to their browser to authorize the app
	void StartLoginRequest(UObject* _WorldContextObject);

	// Starts the API request to get the user's username
	void StartUsernameRequest();

	// Response andler the username API call
	void HandleUsernameHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	// Timeout handler - ensures we don't get stuck forever waiting for the user to complete auth
	void HandleTimeout();

private:
	// Helper function to clean up the routes & timer. Should be called if no further actions are required of the node
	void PerformCleanup();

private:

	// Timer handle for the timeout detection timer
	FTimerHandle TimeoutTimerHandle;

	// World context object (so we can get the world/timer manager)
	UObject* WorldContextObject;

	// Intermediary variable to store the OAuth access token while we execute a HTTP request to figure out the username it belongs to
	FString AccessToken = TEXT("");

	// The HttpRouter we are using for the localhost server - allows for easy unbinding of routes upon cleanup.
	TSharedPtr<IHttpRouter> HttpRouter;

	// Copies of the route handlers so we can unbind them without having to stop all listening processes (which may interfere with external code -> we cannot stop just one listener?)
	FHttpRouteHandle CallbackRouteHandle;
	FHttpRouteHandle TokenSubmissionRouteHandle;

	// Simple flag to store whether cleanup (e.g. unbinding of routes) has been previously completed -> avoids it firing twice.
	bool bCleanupDone = false;

	// "Hard-coded" constants used
private:
	// The page/path (on localhost) to listen to in the callback
	FString ListenPath = TEXT("/twitch_callback");

	// The endpoint where we will listen for an incoming access_token being sent by the JS code on the callback page.
	// NOTE: Has to be a seperate endpoint as the hash Twitch uses to return the token isn't sent to the server.
	// NOTE: This is hard-coded in to the JS code, so update with caution.
	FString SubmitTokenPath = TEXT("/submit_token");
};

// A basic web page with some JS code that extracts the token from the hash and sends it to the /submit_token endpoint.
// NOTE: Hard-coded in as a define to avoid the complexities of loading binary files and forcing them to be packaged with the game. This should be doable, but for the experimental phase, this should be fine.
// NOTE2: The /submit_token endpoint is currently "hard-coded" in, and won't be pulled from the SubmitTokenPath endpoint.
//#define CALLBACK_WEB_PAGE 