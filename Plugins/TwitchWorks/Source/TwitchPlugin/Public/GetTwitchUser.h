// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Engine/Texture2D.h"
#include "TwitchChatConnector.h"
#include "GetTwitchUser.generated.h"

// TODO: Move all data definitions to a single file? 

/*
* Struct that holds the information about a Twitch channel follow (NOTE: Only contains basic information)
* NOTE: Missing data primarily related to logo's/images to save/optimize the quick getting of followers. Having to fetch those resources for all channels would yield (really!) long loading times.
*/
USTRUCT(BlueprintType)
struct FTwitchChannelFollow
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FString Id;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FString DisplayName;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FDateTime FollowDate;

	/* Depricated since they are no longer provided (directly) under the Helix API / aren't the most important pieces of info to know when querying data about a user anyway
	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	int32 NumFollowers;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FDateTime CreationDate;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FString ChannelActiveGame;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	FString ChannelStatus;
	*/
};

/*
 *	Struct that holds the result of a twitch user lookup.
 */
USTRUCT(BlueprintType)
struct FTwitchUserAccount
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString Id;

	//UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	//FString Bio;

	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FDateTime CreationDate;

	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString DisplayName;

	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	UTexture2D *Logo;

	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString UserName;

	// TODO Serialize this into an enum in the future?
	UPROPERTY(BlueprintReadWrite, Category="TwitchWorks", EditAnywhere)
	FString UserType;

	UPROPERTY(BlueprintReadWrite, Category = "TwitchWorks", EditAnywhere)
	TArray<FTwitchChannelFollow> Following;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTwitchGetUserDelegate, FTwitchUserAccount, TwitchUser);


/**
* Task class for getting a twitch user.
*/
UCLASS()
class TWITCHPLUGIN_API UGetTwitchUser : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Gets the information about a twitch user by username.
	*
	* @param Chat The chat that should be used for OAuth credentials.
	* @param UserName The username of the user to get information about.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UGetTwitchUser *GetTwitchUser(class UTwitchChatConnector *Chat, FString UserName);

	virtual void Activate() override;

public:
	// Callback delegate that gets called when we have successfully retrieved the user.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetUserDelegate OnSuccess;
	// Callback delegate that gets called when something goes wrong.
	UPROPERTY(BlueprintAssignable)
	FTwitchGetUserDelegate OnFail;

public:
	// Internal Helper Function - Creates a user lookup HTTP request.
	void CreateHTTPRequest(FString UserName);

	// Internal Helper Function - Creates a user logo lookup request.
	void CreateLogoHTTPRequest(FString LogoURL);

	// Internal Helper Function - Creates a user followers lookup HTTP request.
	void CreateFollowsHTTPRequest(FString UserID, int32 Offset = 0);

private:
	// Internal Helper Function - Handler for user lookup HTTP request callback
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	// Handle success/failure of the logo HTTP request
	void OnSuccessfulLogoFetch(UTexture2D* Logo);
	void OnFailedLogoFetch(UTexture2D* Logo);

	// Internal Helper Function - Handler for user's following list lookup HTTP request callback
	void HandleFollowingHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	// The passed in reqeusted user's username
	FString Username = TEXT("");
	// Reference to the chat used for OAuth credential stuff
	class UTwitchChatConnector* Chat = nullptr;
	FString ExtractedOAuthToken = TEXT("");
	
	// Local variable to keep a track of the user's data while waiting for the logo request to complete.
	FTwitchUserAccount TwitchUser;
};
 