#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "TwitchLoginTask.h"
#include "TwitchGetChatters.generated.h"

USTRUCT()
struct TWITCHAPI_API FGetChattersResponseData
{
	GENERATED_BODY();

	UPROPERTY()
	FString user_id;

	UPROPERTY()
	FString user_login;

	UPROPERTY()
	FString user_name;
};

USTRUCT()
struct TWITCHAPI_API FGetChattersResponse
{
	GENERATED_BODY();

	UPROPERTY()
	TArray<FGetChattersResponseData> data;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetChattersDelegate, const TArray<FString>&, Viewers);

/**
 * Task class for getting the current chatters/logged in viewers of a channel.
 */
UCLASS()
class TWITCHAPI_API UTwitchGetChatters : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Gets all logged in chatters.
	*
	* @return The usernames of all chatters/logged in viewers of the given channel
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UTwitchGetChatters* GetChatters(const UObject* worldContextObject);

public:
	// Callback delegate that gets called when the list is successfully retrieved.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetChattersDelegate OnSuccess;

	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetChattersDelegate OnFail;

public:
	// Internal Helper Function - Creates a subscription lookup HTTP request.
	void MakeRequest(const FTwitchLoginCredentials& Credentials);

	// Internal Helper Function - Handler for subscription lookup HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};