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

USTRUCT(Blueprintable)
struct TWITCHAPI_API FChatter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int64 UserId;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString UserName;
};

inline bool operator==(const FChatter& Lhs, const FChatter& Rhs)
{
	return Lhs.UserId == Rhs.UserId;
}

inline uint32 GetTypeHash(const FChatter& Chatter)
{
	return GetTypeHash(Chatter.UserId);
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetChattersDelegate, const TArray<FChatter>&, Chatters);

/**
 * Task class for getting the current chatters of a channel.
 */
UCLASS()
class TWITCHAPI_API UTwitchGetChatters : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Gets all logged in chatters.
	*
	* @return All chatters of the given channel
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