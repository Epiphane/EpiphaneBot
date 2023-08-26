// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TwitchAPITypes.generated.h"

/*
 *  REQUEST TYPES
 */
USTRUCT()
struct FEventSubscriptionCondition
{
	GENERATED_BODY()

	UPROPERTY()
	FString broadcaster_user_id;

	UPROPERTY()
	FString moderator_user_id;
};

USTRUCT()
struct FEventSubscriptionTransport
{
	GENERATED_BODY()

	UPROPERTY()
	FString method = TEXT("websocket");

	UPROPERTY()
	FString session_id;
};

USTRUCT()
struct FEventSubscriptionRequest
{
	GENERATED_BODY()

	UPROPERTY()
	FString type;

	UPROPERTY()
	FString version;

	UPROPERTY()
	FEventSubscriptionCondition condition;

	UPROPERTY()
	FEventSubscriptionTransport transport;
};

/*
 *  RESPONSE TYPES
 */
UENUM()
enum class ETwitchEventMessageType : uint8
{
	session_welcome = 0,
	session_keepalive,
	session_reconnect,
	notification
};

USTRUCT()
struct TWITCHAPI_API FTwitchEventMetadata
{
	GENERATED_BODY()

	UPROPERTY()
	FString message_id;

	UPROPERTY()
	ETwitchEventMessageType message_type;

	UPROPERTY()
	FString message_timestamp;

	UPROPERTY()
	FString subscription_type;

	UPROPERTY()
	FString subscription_version;
};

UENUM()
enum class ETwitchSessionStatus : uint8
{
	connected = 0,
	reconnecting,
};

USTRUCT()
struct TWITCHAPI_API FTwitchEventSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString id;

	UPROPERTY()
	ETwitchSessionStatus status;

	UPROPERTY()
	int keepalive_timeout_seconds;

	UPROPERTY()
	FString reconnect_url;

	UPROPERTY()
	FString connected_at;
};

USTRUCT()
struct TWITCHAPI_API FTwitchEventSessionPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FTwitchEventSession session;
};

USTRUCT(Blueprintable)
struct TWITCHAPI_API FChannelPointAPIRedemption
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString id;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString title;
};

USTRUCT()
struct TWITCHAPI_API FRedemptionNotification
{
	GENERATED_BODY()

	UPROPERTY()
	FString id;

	UPROPERTY()
	FString user_id;

	UPROPERTY()
	FString user_name;

	UPROPERTY()
	FChannelPointAPIRedemption reward;
};
