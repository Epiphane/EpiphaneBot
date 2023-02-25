// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "Containers/Map.h"
#include "TwitchChatConnector.h"
#include "TwitchPluginBPLibrary.generated.h"

/* 
*	Twitch Blueprint Function Library Class
*/
UCLASS()
class UTwitchPluginBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Sends a message to a twitch chat.
	*
	* @param Chat The twitch chat to send the message to.
	* @param Message The message to send.
	*
	* @return true if the message was successfully sent, false otherwise.
	*/
	static bool SendTwitchMessage(UTwitchChatConnector* Chat, FString Message);

	/**
	* Sends a message to a twitch chat.
	*
	* @param Chat The twitch chat to send the message to.
	* @param Message The message to send.
	*
	* @return true if the message was successfully sent, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Twitch Message", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool SendTwitchMessage(UTwitchChatConnector *Chat, FText Message);

	/**
	* Permanently bans a twitch user.
	*
	* @param Chat The twitch chat to use.
	* @param Username The username of the user to ban.
	* @param Reason The reason why you are banning the user (optional)
	*
	* @return true if the user was successfully banned, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Parmanently Ban Twitch User", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool ParmaBanTwitchUser(UTwitchChatConnector *Chat, FString Username, FString Reason = TEXT(""));

	/**
	* Temporarily bans a twitch user.
	*
	* @param Chat The twitch chat to use.
	* @param Username The username of the user to ban.
	* @param Duration The amount of seconds to ban the user.
	* @param Reason The reason to ban the user.
	*
	* @return true if the user was successfully banned, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Temp Ban Twitch User", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool TempBanTwitchUser(UTwitchChatConnector *Chat, FString Username, int32 Duration /* In Seconds */, FString Reason);

	/**
	* Enables twitch slow chat.
	*
	* @param Chat The twitch chat to apply slow chat to.
	* @param Seconds Interval between messages in seconds.
	*
	* @return true if slow chat was successfully enabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Twitch Slow Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool EnableSlowChat(UTwitchChatConnector *Chat, int32 Seconds /* Seconds between mesages */);

	/**
	* Disables twitch slow chat.
	*
	* @param Chat The twitch chat to apply slow chat to.
	*
	* @return true if slow chat was successfully disabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Twitch Slow Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool DisableSlowChat(UTwitchChatConnector *Chat);

	/**
	* Enables twitch emote only chat.
	*
	* @param Chat The twitch chat to apply emote only chat to.
	*
	* @return true if emote only chat was successfully enabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Twitch Emote Only Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool EnableEmoteChat(UTwitchChatConnector *Chat);

	/**
	* Disables twitch emote only chat.
	*
	* @param Chat The twitch chat to disable emote only chat on.
	*
	* @return true if emote only chat was successfully disabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Twitch Emote Only Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool DisableEmoteChat(UTwitchChatConnector *Chat);

	/**
	* Enables twitch R9K chat.
	* @warning R9K chat is still in beta!
	*
	* @param Chat The twitch chat to apply R9K chat to.
	*
	* @return true if R9K chat was successfully enabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Twitch R9K Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool EnableR9KChat(UTwitchChatConnector *Chat);

	/**
	* Disables twitch R9K chat.
	*
	* @param Chat The twitch chat to disable R9K chat on.
	*
	* @return true if emote only chat was successfully disabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Twitch R9K Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool DisableR9KChat(UTwitchChatConnector *Chat);

	/**
	* Clears a twitch chat.
	*
	* @param Chat The Twitch chat to clear.
	*
	* @return true if the chat was successfully cleared, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clear Twitch Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool CearChat(UTwitchChatConnector *Chat);

	// Example usage: Only followers who have been following for a while can vote or smth! NOTE: having 0 will simply limit the chat to followers
	/**
	* Enables followers only chat.
	*
	* @param Chat The Twitch chat to apply followers only to.
	* @param followDuration The duration the user must be a follower, max is 3 months.
	*
	* @return true if the followers only was successfully enabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Enable Followers Only Twitch Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool EnableFollowersOnlyChat(UTwitchChatConnector *Chat, FTimespan followDuration /* Up to 3 months - followers need to have followed this channel for how long? */);

	/**
	* Disables followers only chat.
	*
	* @param Chat The Twitch chat to disable followers only on.
	*
	* @return true if followers only chat was successfully disabled, false otherwise
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disable Followers Only Twitch Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool DisableollowersOnlyChat(UTwitchChatConnector *Chat);

	// TODO don't forget to update the below functions if usergroup (=mod/sub/etc.) specific-commands are a thing

	/**
	* Creates a twitch command.
	*
	* @param Chat The chat to which to listen to for the command.
	* @param CommandBinding The command/binding to use.
	*
	* @return true if the command was successfully registered, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create Twitch Command", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool CreateTwitchCommand(UTwitchChatConnector *Chat, FString CommandName, FText CommandDescription, FText Usage, int NumParameters, bool bShowsUpInHelp, FTwitchCommandDelegate Binding);

	/**
	* Updates a twitch command.
	*
	* @param Chat The chat to which the command is bound.
	* @param CommandName The name of the (old) command.
	* @param NewCommandBinding The new command/binding to use.
	*
	* @return true if the command was successfully changed, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Twitch Command", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool UpdateTwitchCommand(UTwitchChatConnector *Chat, FString CommandName, FTwitchCommandBinding NewCommandBinding);

	/**
	* Updates a twitch command.
	*
	* @param Chat The chat to which the command is bound.
	* @param CommandName The name of the old command you wish be be unbound.
	*
	* @return true if the command was successfully removed, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove/Unbind From Twitch Command", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool RemoveTwitchCommand(UTwitchChatConnector *Chat, FString CommandName);

	/**
	* Gets the registered twitch commands.
	*
	* @param Chat The chat to find the commands for.
	* @param Commands The commands that where found.
	*
	* @return true if we successfully got the commands, false otherwise.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Active Twitch Commands", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool GetRegisteredTwitchCommands(UTwitchChatConnector *Chat, TArray<FTwitchCommandBinding>& Commands);

	// NOTE: Must be set up in settings to work!
	/**
	* Binds to a twitch command set up in the settings menu.
	*
	* @param Chat The chat to which to listen to for the command.
	* @param CommandName The name of the command you wish to bind to.
	* @param Delegate The event you wish to bind to the command.
	*
	* @return true if the command was successfully bound, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind To Twitch Command", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame"), Category = "TwitchWorks")
	static bool BindToTwitchCommand(UTwitchChatConnector *Chat, FString CommandName, FTwitchCommandDelegate Delegate);

	/**
	* Disconnects from a Twitch chat and destroys the task in charge of maintaining connection.
	*
	* @param Chat The chat you wish to disconnect from.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disconnect from Chat", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame disconnect chat chatdisconnect"), Category = "TwitchWorks")
	static void DisconnectFromChat(UTwitchChatConnector *Chat);

	/**
	* Binds an event to the new subscriber/resubscriber/subscription gifted event.
	*
	* @param Chat The chat you wish to bind to.
	* @param Event The event you wish to execute when a (renewal of a) subscription occurs.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind to OnNewSubscription", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame bind chat subscribers newsub sub"), Category = "TwitchWorks")
	static void BindToOnNewSubscription(UTwitchChatConnector *Chat, FTwitchNewSubscriptionDelegate Event);

	/**
	* Binds an event to the ritual event.
	*
	* @param Chat The chat you wish to bind to.
	* @param Event The event you wish to execute when a ritual occurs.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind to OnRitual", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame bind chat rituals netritual"), Category = "TwitchWorks")
	static void BindToOnRitual(UTwitchChatConnector *Chat, FTwitchRitualDelegate Event);

	/**
	* Binds an event to the points redemption event.
	* WARNING: This feature is in EARLY ACCESS, and may not work the way you expect it to. Please use with caution.
	*
	* @param Chat The chat you wish to bind to.
	* @param Event The event you wish to execute when a point redemption event occurs.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind to OnPointsEvent (EA)", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame bind chat points channelpoints channel"), Category = "TwitchWorks")
	static void BindToOnPointsEvent(UTwitchChatConnector *Chat, FTwitchPointsEventDelegate Event);

	/**
	* Binds an event to the raid event.
	*
	* @param Chat The chat you wish to bind to.
	* @param Event The event you wish to execute when a raid occurs.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind to OnRaid", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame bind chat raid raids raiders"), Category = "TwitchWorks")
	static void BindToOnRaid(UTwitchChatConnector* Chat, FTwitchRaidEventDelegate Event);

	/**
	* Gets the "raw" identifier of a badge. Can be useful to identify unknown badges.
	*
	* @param Badge The badge to parse
	*
	* @return The badge's identifier if found, or a blank string if it could not be found.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Badge Identifier", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame badge badges badgecode"), Category = "TwitchWorks")
	static FString GetBadgeIdentifier(FTwitchBadge Badge);

	/**
	* Binds an event to the whisper receive event.
	*
	* @param Chat The chat you wish to bind to.
	* @param Event The event you wish to execute when a whisper is received.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind to OnWhisper", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame bind chat whisper private pm privatemessage message onwhisper whisperreceived received"), Category = "TwitchWorks")
	static void BindToOnWhisper(UTwitchChatConnector* Chat, FTwitchWhisperDelegate Event);

	/**
	* Obtains the current chat modes.
	*
	* @param Chat The chat to get the information about.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Chat Modes", Keywords = "Twitch TwitchWorks TwitchPlugin Livestream Livestreaming TwitchGame r9k slow emote-only sub-only state mode chat chatmode"), Category = "TwitchWorks")
	static FTwitchChatMode GetChatModes(UTwitchChatConnector* Chat);
};
