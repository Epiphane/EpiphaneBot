// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Containers/Map.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Async/AsyncWork.h"
#include "TwitchLoginTask.h" // Twitch login credentials struct TODO create a types class
#include "Components/ActorComponent.h"
#include "TwitchRuntimeSettings.h" // Settings
#include "GetTwitchEmoteSet.h"
#include "IWebSocket.h"
#include "TwitchChatConnector.generated.h"

// Pre-declare it so the component can compile
class FTwitchChatConnectorTask;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatFeedDelegate, FString, Message);
DECLARE_DELEGATE_OneParam(FChatFeedCallbackDelegate, FString); // Why is this needed? To bind ufunction? Because other delegate doesn't work?

// Twitch Badges
UENUM(BlueprintType, Meta=(ScriptName="TwitchBadgeType"))
enum class ETwitchBadge : uint8
{
	TB_ADMIN,
	TB_BITS,
	TB_BROADCASTER,
	TB_GLOBALMOD,
	TB_MOD,
	TB_SUBSCRIBER,
	TB_STAFF,
	TB_TURBO,
	// Not all tested yet! TODO Taken off of https://help.twitch.tv/customer/portal/articles/659115-chat-icons-and-emoticons and not the docs, so unsure about text representation accuracy - TODO also check out https://discuss.dev.twitch.tv/t/beta-badge-api/6388 / https://badges.twitch.tv/v1/badges/global/display?language=en
	TB_PARTNER, // Verified
	TB_PREMIUM,  // Prime
	TB_TWITCHBOT, // AutoMod
	TB_VIP, // VIP User Badge
	TB_SUBGIFTER, // Sub Gifter badge

	TB_UNKNOWN = 255, // Default/built-in badge that will show be used when an unknown badge is detected.

};

// Twitch User Types
UENUM(BlueprintType)
enum class ETwitchUserType : uint8
{
	TUT_EMPTY,
	TUT_MOD,
	TUT_GLOBALMOD,
	TUT_ADMIN,
	TUT_STAFF
};

// Twitch Subscription Types
UENUM(BlueprintType)
enum class ETwitchSubscriptionType : uint8
{
	// Prime subscription
	TST_PRIME,
	// First subscription tier ($4.99)
	TST_First,
	// Third subscription tier ($9.99)
	TST_Second,
	// Third subscription tier ($24.99)
	TST_Third,
	// Unknown subscription tier (not recognized)
	TST_UNKNOWN
};

// Struct Holding The Badge & Version
USTRUCT(BlueprintType)
struct FTwitchBadge
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	ETwitchBadge Badge;

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	int32 Version; // TODO should this be an int32? Can badges be floats or have letters?

	// The badge's code to enable unknown badges to be read/interpreted/used more eaisly. Kept seperately from the existing ETwitchBadge system to ensure backwards compatibility.
	// NOTE: Not a uproperty as they should not show up on the struct in BP (should be fetched using a dedicated (pure) node to avoid confusing users with 2 properties on a struct that are virtually identical).
	FString RawBadgeCode;
};

// Twitch Message Author/Sender
USTRUCT(BlueprintType)
struct FTwitchMessageAuthor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	TArray<FTwitchBadge> Badges;
	//TODO bits, should these be in author or message & how should c++ parse 'em?
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FColor Color;
	// @warning DisplayName is not always set!
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString Name; 
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	bool bIsMod;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	bool bIsTurbo;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	bool bIsSubscriber;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString UserId;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	ETwitchUserType UserType;
};

// Twitch Message Struct
USTRUCT(BlueprintType)
struct FTwitchMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FTwitchMessageAuthor Author;
	UPROPERTY()
	FString MessageId; // warning: 	The type of notice (not the ID). Valid values: sub, resub, charity.
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString MessageBody;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FString RoomId;

	// The locations (X: start, Y: end) in the text that are emoticons. EmoteSet won't be valid.
	// NOTE: FIntPoint chosen to avoid having to declare our own structure (TPair/TTuple aren't BP-compatible)
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	TMap<FIntPoint, FTwitchEmoticon> Emotes;
};

// "Abstracted" Message, Holds Bits Cheered & Message Sent
USTRUCT(BlueprintType)
struct FTwitchCheer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FTwitchMessage Message;

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	int32 Bits; // # Bits Cheered
};

// "Abstracted" Message that was sent as part of a channel points reward
USTRUCT(BlueprintType)
struct FTwitchPointsEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FTwitchMessage Message;

	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString RewardID; // The custom reward ID. Reward must have "Require Viewer to Enter Text" enabled.
};

// Twitch New Subscription Struct. Inherets from the standard Twitch Message sicne it shares a lot in common.
USTRUCT(BlueprintType)
struct FTwitchSubscription : public FTwitchMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bWasNewSub;
	
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	bool bWasGift;

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	bool bWasResub;

	// NOTE: Below values only apply when the subscription was not a gift.
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	int32 NumMonthsSubscribed;

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	ETwitchSubscriptionType SubscriptionType;

	// Only relevant for the gift: the username of the user who the subscription was gifted to.
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString GiftRecipientUsername;

	// Only relevant for when a user donates multiple subs to the channel; set to -1 if not the case, but other number if it is
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	int SubGiftSize = -1;
};

// Twitch Raid Event Struct. Includes information about an incoming raid
USTRUCT(BlueprintType)
struct FTwitchRaidEvent
{
	GENERATED_BODY()

	/**
	* The display name of the user that is (about to) raid the logged in user's channel.
	*/
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString RaiderDisplayName;
	
	/**
	* The username of the user that is (about to) raid the logged in user's channel.
	*/
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	FString RaiderUsername;

	/**
	* The number of viewers/raiders that will be coming to the logged in user's channel.
	*/
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	int NumRaiders;
};

// Twitch User - You (The Person Who Logged In)
USTRUCT(BlueprintType)
struct FTwitchUser
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
		FColor Color;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
		FString DisplayName;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
		TArray<int32> EmoteSets;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
		bool bIsTurbo;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
		FString UserId;
	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
		ETwitchUserType UserType;
};

// The Twitch chat mode / room state.
USTRUCT(BlueprintType)
struct FTwitchChatMode
{
	GENERATED_BODY()

	// Whether or not the chat is in emote-only mode
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bEmoteOnly;

	// Whether or not the chat is in followers-only mode
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bFollowersOnly;

	// Whether or not the chat is in R9K mode
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bR9KEnabled;

	// The minimum time non-mods should wait between messages
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	float SlowTime;

	// If sub-only chat is enabled
	UPROPERTY(EditAnywhere, Category = "TwitchWorks", BlueprintReadWrite)
	bool bSubOnly;
};

// TODO/future idea: make delegates for 0-9 parameters so we don't have to pass a Parameters array.
DECLARE_DYNAMIC_DELEGATE_TwoParams(FTwitchCommandDelegate, const TArray<FString>&, Parameters, FTwitchMessageAuthor, Sender);

// Holds a bound or unbound TwitchCommand
USTRUCT(BlueprintType)
struct FTwitchCommandBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FTwitchCommand Command;

	UPROPERTY(EditAnywhere, Category="TwitchWorks", BlueprintReadWrite)
	FTwitchCommandDelegate Binding;

	// Overload the == operator for searching/using remove
	bool operator==(const FTwitchCommandBinding& Other) const
	{
		return Command.CommandName == Other.Command.CommandName;
	}
};

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatMessageSentDelegate, FTwitchMessage, TwitchMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLoginStateChangedEvent, FTwitchUser, TwitchUser, UTwitchChatConnector*, TwitchChat);
DECLARE_DYNAMIC_DELEGATE_OneParam(FChatMessageSentDelegateTwo, FTwitchMessage, TwitchMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTwitchWhisperDelegate, FTwitchMessage, TwitchMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTwitchCheerDelegate, FTwitchCheer, TwitchCheer);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTwitchPointsEventDelegate, FTwitchPointsEvent, PointsEvent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTwitchRaidEventDelegate, FTwitchRaidEvent, RaidEvent);
DECLARE_DELEGATE_OneParam(FReferenceCallbackDelegate, FTwitchChatConnectorTask*);
DECLARE_DYNAMIC_DELEGATE_OneParam(FTwitchNewSubscriptionDelegate, FTwitchSubscription, TwitchSubscription);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FTwitchRitualDelegate, FTwitchMessage, TwitchMessage, FString, RitualName);

/*
 * 
 */

// Class for asynchronously creating a twitch (chat) connection.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TWITCHPLUGIN_API UTwitchChatConnector : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Connects to twitch (chat).
	*
	* @param OnMessage Event to call when a message has been received.
	* @param OnCheer Event to call when a cheer has been received.
	* @param Credentials The credentials (username/oauth token) used to log in.
	* @param Channel The twitch channel (lower case) to connect to, usually equal to the username.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName="Connect To Twitch", BlueprintInternalUseOnly="true"))
	static UTwitchChatConnector* RequestFeed(FChatMessageSentDelegateTwo OnMessage, FTwitchCheerDelegate OnCheer, FTwitchLoginCredentials Credentials, FString Channel);

	// Deconstruction - Stops async task
	virtual void BeginDestroy() override;

public:
	// Gets called when the twitch login was successful
	UPROPERTY(BlueprintAssignable)
	FLoginStateChangedEvent OnSuccessfulLogin;

	// Gets called when there was an issue connecting to the twitch api
	UPROPERTY(BlueprintAssignable)
	FLoginStateChangedEvent OnFailedLogin;

	// OnMessage/OnCher delegates
	FChatMessageSentDelegateTwo OnMessage;
	FTwitchCheerDelegate OnCheer;

	// Subscription, ritual and point event and whisper event delegates
	// NOTE: These aren't BlueprintAssignable as that would expose them on the left-hand side of the BP -> making them required rather that optionnal (the desired result)
	FTwitchNewSubscriptionDelegate OnNewSubscription;
	FTwitchRitualDelegate OnRitual;
	FTwitchPointsEventDelegate OnPointsEvent;
	FTwitchRaidEventDelegate OnRaidEvent;
	FTwitchWhisperDelegate OnWhisperEvent;

	// Reference to the async task that actually maintains the socket connection so we can send messages etc. later.
	FTwitchChatConnectorTask *AsyncWorkTask = nullptr;
	
	// The channel this chat is connected to (used for IRC traffic).
	FString Channel;

	// The commands that have been created and/or bound to this chat.
	TArray<FTwitchCommandBinding> Commands;
	// TODO encapsulate all of these public variables

	// Variable to help this not crash when it is disconnected by the BP node.
	bool bIsChatActive = true;
	
	// Vraiable to store the credentials of the connected user in case we need to ever access them.
	FTwitchLoginCredentials ConnectedUserCreds;

	// The current ROOMSTATE (not a UPROPERTY to ensure users must go through the function/to ensure a more uniform interface)
	FTwitchChatMode ChatMode;

public:
	// Internal Function - Creates & logs into an IRC socket connection.
	void Start(FTwitchLoginCredentials Credentials, FString Channel);
	
	// Internal Helper Function - Handles the parsing & delegates around twitch messages.
	void TwitchIRCMessageRecieved(FString Message);

	// Internal Helper Function - Parses a (twitch message) buffer using the format string.
	TArray<FString> ParseTwitchMessage(FString Buffer, FString Format);

	// Internal Helper Function - Parses the twitch tags in a tag buffer.
	TMap<FString, FString> ParseTwitchTags(FString Buffer);

	// Internal Helper Function - Extracts the badges from a twitch tags buffer.
	TArray<FTwitchBadge> GetTwitchBadges(TMap<FString, FString> Tags);
	// Internal Helper Function - Extracts the emote sets from a twitch tags buffer.
	TArray<int32> GetEmoteSets(TMap<FString, FString> Tags);

	// Callback to keep a reference to the async task
	void ReferenceCallbackFunction(FTwitchChatConnectorTask* Task);

	// Callback for when the chat connector fails to connect; please ensure that Task is not used as it will be a nullptr
	void OnChatConnectionFailed(FTwitchChatConnectorTask* Task);

	// Gets called when a twitch message has been successfully parsed - checks it for being a command binding and/or executes the correct delegates
	void OnTwitchMessageRecieved(FTwitchMessage TwitchMessage);

	// Internal Helper Function - Extracts the arguments out of a twitch command buffer.
	TArray<FString> ParseTwitchCommand(FString Buffer, FTwitchCommand Command);

	// Helper funmction for parsing emote positions/IDs from chat messages. NOTE: EmoteSet won't be set!
	TMap<FIntPoint, FTwitchEmoticon> ParseEmotesFromMessage(FString MessageBody, FString EmotesTag);
	
};

class FTwitchChatConnectorTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FTwitchChatConnectorTask>;

public:
	FTwitchChatConnectorTask(FChatFeedCallbackDelegate OnMessageRecievedDelegate, FReferenceCallbackDelegate referenceCallbackDelegate, UTwitchChatConnector *Caller, FTwitchLoginCredentials Credentials, FString TwitchChannel, FReferenceCallbackDelegate OnFailureDelegate)
		:
		OnMessageRecieved(OnMessageRecievedDelegate),
		OnFailure(OnFailureDelegate),
		bIsTaskActive(true),
		Channel(TwitchChannel),
		TwitchCredentials(Credentials),
		Target(Caller)
	{
		// Call back the caller with a reference to the actual task (this).
		referenceCallbackDelegate.ExecuteIfBound(this);
	}

	// Will attempt to send a message (string, it won't do IRC formatting for you) over the socket connection.
	bool SendOverSocket(FString Message);

	// Will stop the task the next time the DoWork function makes an iteration.
	void StopTask()
	{
		// Set the task to inactive (which should stop new interactions) and disconnect from TMI to avoid getting more stuff in.
		bIsTaskActive = false;
		WebSocket->Close();
	}

protected:
	// Shadowed DoWork() function - main action of the async task.
	void DoWork();

	void OnTwitchMessage(const FString& Message);
	void OnTwitchConnected();

	void OnConnectionError(const FString& Error);

	void OnClosed(int StatusCode, const FString& Error, bool bWasClean);

	
	// Gets called when an IRC message/command/buffer gets received.
	FChatFeedCallbackDelegate OnMessageRecieved;
	
	// Gets called when there is some form of connection error
	// NOTE: We don't need the task reference param, but this allows us to get away with not defining yet another delegate
	FReferenceCallbackDelegate OnFailure;

	// FNonAbandonableTask specific task code.
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTwitchChatConnectorTask, STATGROUP_ThreadPoolAsyncTasks);
	}
public:
	// Is the task currently running?
	bool bIsTaskActive;
private:
	// The channel that this chat task is connected to.
	FString Channel;
	// The twitch credentials required to log in to twitch.
	FTwitchLoginCredentials TwitchCredentials;
	// The chat target (caller/creator of this task).
	UTwitchChatConnector *Target;


	// WebSocket maintaining the connection with Twitch
	TSharedPtr<IWebSocket> WebSocket;

};