// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IWebSocket.h"
#include "TwitchAPITypes.h"
#include "TwitchLoginTask.h"
#include "TwitchGetChatters.h"
#include "TwitchPubSubConnection.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FChannelPointRedemptionDelegate, FChannelPointAPIRedemption, Redeem, FString, UserId, FString, UserName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectedToChannelDelegate, FString, Channel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisconnectedFromChannelDelegate);

UCLASS()
class TWITCHAPI_API UTwitchEventsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void OpenConnection(FTwitchLoginCredentials Credentials);

public:
	const FTwitchLoginCredentials& GetCredentials() const { return Credentials; }

	virtual void Deinitialize() override;

	void SubscribeToEvent(FString Type, FString Version);

	UFUNCTION(BlueprintCallable)
	void SubscribeToEvents();
	void HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
public:
	FChannelPointRedemptionDelegate& GetOnChannelPointRedemption() { return OnChannelPointRedemption; }
	FConnectedToChannelDelegate& GetOnConnected() { return OnConnected; }
	FDisconnectedFromChannelDelegate& GetOnDisconnected() { return OnDisconnected; }

private:
	void OnSocketConnected();
	void OnConnectionError(const FString& Error);
	void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	void OnMessage(const FString& Message);
	void OnNotification(FString Type, TSharedPtr<FJsonObject> Payload);
	void OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);
	void OnMessageSent(const FString& MessageString);

private:
	UPROPERTY(BlueprintAssignable)
	FChannelPointRedemptionDelegate OnChannelPointRedemption;

	UPROPERTY(BlueprintAssignable)
	FConnectedToChannelDelegate OnConnected;

	UPROPERTY(BlueprintAssignable)
	FDisconnectedFromChannelDelegate OnDisconnected;

private:
	FTwitchLoginCredentials Credentials;
	TSharedPtr<IWebSocket> Socket;
	FString SessionID;
};
