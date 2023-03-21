// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "TwitchRuntimeSettings.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTwitchCommandParameter
{
	GENERATED_BODY()

	// The name of the parameter (used in help menu).
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString ParameterName;

	// The description of the parameter (used in help menu).
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString ParameterDescription;

	// The parameter's delimiter (the character to be hit before it should go to the next command, eg. <space>, |, etc.)
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString ParameterDelimiter = TEXT(" ");
};

USTRUCT(BlueprintType)
struct FTwitchCommand
{
	GENERATED_BODY()
	
	// The command's name.
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString CommandName;

	// The command's description (used in help menu).
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FText CommandDescription;

	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FText Usage;

	// The command's parameters
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	TArray<FTwitchCommandParameter> Parameters;

	// Should this command show up in the auto-generated help message?
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	bool showsUpInHelp;
};

UCLASS(config = Project, defaultconfig)
class TWITCHSETTINGSPLUGIN_API UTwitchRuntimeSettings : public UObject
{
	GENERATED_BODY()
	
public:
	// Your twitch app's Client-ID
	UPROPERTY(EditAnywhere, config, Category = Authentication)
	FString clientID;

	// The game's API part that generates & registers a token/key
	UPROPERTY(EditAnywhere, config, Category = Authentication)
	FString twitchLoginApiURL;

	// The game's API part that redirects the user to twitch
	UPROPERTY(EditAnywhere, config, Category = Authentication)
	FString twitchLoginURL;

	// The number of seconds after starting the login procedure beyond which the login is considered failed or -1 for no timeout. NOTE: users may need to log in/complete 2FA, so don't make this too short!
	UPROPERTY(EditAnywhere, config, Category = Authentication)
	float twitchLoginTimeout = -1;
	
	// Should it cache emoticon requests? (HIGHLY recommended)
	UPROPERTY(EditAnywhere, config, Category = Caching)
	bool shouldCacheEmoteRequests = true;

	// Should TwitchWorks cache the document containing all badge URLs? (HIGHLY recommended)
	UPROPERTY(EditAnywhere, config, Category = Caching)
	bool shouldCacheBadgeURLs = true;

	// Should TwitchWorks cache badge images? (HIGHLY recommended)
	UPROPERTY(EditAnywhere, config, Category = Caching)
	bool shouldCacheBadgeImages = true;

	// The timeout after which an emote fetching request should fail (in seconds; use -1 for no timeout)
	UPROPERTY(EditAnywhere, config, Category = Experimental)
	int emoteFetchTimeout = -1;


	// *LOCAL* LOGIN! How long should we wait for the user to finish logging in before we deem the request timed out and fail it?
	UPROPERTY(EditAnywhere, config, Category = ExperimentalLocalLogin)
	float localLoginTimeout = 60.f;

	// *LOCAL* LOGIN! Which port should we start the localhost callback web server on?
	UPROPERTY(EditAnywhere, config, Category = ExperimentalLocalLogin)
	uint32 localLoginPort = 8080;

	// *LOCAL* LOGIN! Which port should we start the localhost callback web server on?
	// user:read:email+chat:read+chat:edit+channel:moderate+whispers:edit+user:read:subscriptions
	UPROPERTY(EditAnywhere, config, Category = ExperimentalLocalLogin)
	TArray<FString> localLoginScopes = { TEXT("user:read:email"), TEXT("chat:read"), TEXT("chat:edit"), TEXT("channel:moderate"), TEXT("whispers:edit"), TEXT("user:read:subscriptions") };

	// *LOCAL* LOGIN! Whether or not to shut down ALL HTTP listeners after shutdown (cannot just stop 1; might break other functionality)
	UPROPERTY(EditAnywhere, config, Category = ExperimentalLocalLogin)
	bool bStopAllListenersOnLocalLoginComplete = false;

	// The commands set up in the menu
	UPROPERTY(EditAnywhere, config, Category = Commands)
	TArray<FTwitchCommand> Commands;

	// The command prefix (!, $, !! etc.)
	UPROPERTY(EditAnywhere, config, Category = Commands)
	FString commandPrefix;

	// Should it auto generate a help message when the help command is executed?
	UPROPERTY(EditAnywhere, config, Category = Commands)
	bool bIsCommandHelpEnabled;

	// What should the help command be? (only applicable if the help command is enabled)
	UPROPERTY(EditAnywhere, config, Category = Commands)
	FString helpCommand = TEXT("help");
};
