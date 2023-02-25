// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "TwitchLoginTask.generated.h"

// Struct to store the twtich login credentials (username/oauth token)
USTRUCT(BlueprintType)
struct FTwitchLoginCredentials
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString OAuthToken;
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString Username;
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString RefreshToken;

	/*
	 * Introduced in TwiWorks 1.0.21 to help with various functions, such as channel point reward management.
	 * REQUIRES THE LATEST BACKEND VERSION/THIS PLUGIN'S LOCAL LOGIN FUNCTION. PLEASE UPDATE IF YOU HAVE NOT DONE SO!
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FString UserId;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchLoginDelegate, FTwitchLoginCredentials, Credentials);

/**
 * 
 */

/*
* Async login task class.
*/
UCLASS()
class TWITCHPLUGIN_API UTwitchLoginTask : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Opens a browser and asks the user to authorize your application, calls back when the user has logged in and switches back.
	*
	* @param WorldContextObject The world context object.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UTwitchLoginTask *PerformTwitchLogin(UObject* WorldContextObject);

	virtual void Activate() override;

public:
	// Gets called when the user successfully logs in
	UPROPERTY(BlueprintAssignable)
	FTwitchLoginDelegate OnSuccess;

	// Gets called when there is an error logging in.
	UPROPERTY(BlueprintAssignable)
	FTwitchLoginDelegate OnFail;

	// Gets called when the login procedure times out
	UPROPERTY(BlueprintAssignable)
	FTwitchLoginDelegate OnTimeout;

public:
	// Stops the async task. Helps solve wierd behavior when the editor gets closed
	virtual void BeginDestroy() override;

public:
	// Gets called by the async task to avoid delegate issues when they are called from async tasks.
	void DoCallback(FString requestKey, FString requestToken, bool bWasSuccessful);

	// Gets called when the focus/api check timer completes
	void DoTimerCallback();

	// Starts the API has user logged in? checks
	void StartPollingRequest();

	// Handler for the login start i.e. token/key generation HTTP request callback
	void HandleStartHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	// Handler for the polling request callback
	void HandlePollingHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	// Handler for when the login procedure times out
	void OnTimeoutReached();
private:
	// Timer handle that stores the timer that checks whether or not the user focussed back to the game every second
	FTimerHandle focusCheckTimerHandle;

	// Time handle starts the polling HTTP requests automagically
	FTimerHandle pollRequestTimerHandle;

	// Timer handle for the timeout timer
	FTimerHandle timeoutTimerHandle;

	// World context object (so we can get the world/timer manager)
	UObject* WorldContextObject;

	// The game's twitch API credentials (PHP script).
	FString requestToken;
	FString requestKey;
};