// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "TwitchChatConnector.h"
#include "RefreshTokenTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchRefreshTokenDelegate, FTwitchLoginCredentials, RefreshedCredentials);

/**
 * Task class for refreshing an access token when it has expired.
 */
UCLASS()
class TWITCHPLUGIN_API URefreshToken : public UBlueprintAsyncActionBase
{
		GENERATED_UCLASS_BODY()

public:
	/**
	* Will (attempt to) refresh a token; should be used when credentials have expired.
	* NOTE: Backend version 1.6+ required!
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static URefreshToken *RefreshCredentials(FTwitchLoginCredentials Credentials);

public:
	// Callback delegate that gets called when a new token has successfully been issued.
	UPROPERTY(BlueprintAssignable)
	FTwitchRefreshTokenDelegate OnRefreshed;
	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchRefreshTokenDelegate OnFail;

public:
	// Internal Helper Function - Creates a refresh token HTTP request.
	void CreateHTTPRequest(FString RefreshToken);

private:
	// Internal Helper Function - Handler for the refresh HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
private:
	// Local variable used to store the user's twitch credentials: this way we can preserve the refresh token/username.
	FTwitchLoginCredentials Credentials;
};