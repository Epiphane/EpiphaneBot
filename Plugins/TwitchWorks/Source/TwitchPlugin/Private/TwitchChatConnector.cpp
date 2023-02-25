// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "TwitchChatConnector.h"
#include "Containers/StringConv.h"
#include "TwitchPlugin.h"
#include "WebSocketsModule.h"

// Twitch chat connector constructior
UTwitchChatConnector::UTwitchChatConnector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UTwitchChatConnector *UTwitchChatConnector::RequestFeed(FChatMessageSentDelegateTwo OnMessage, FTwitchCheerDelegate OnCheer, FTwitchLoginCredentials Credentials, FString Channel)
{
	// Create a new instance of this class that we can return/mutate and set it's delegates to the passed in delegates
	UTwitchChatConnector *RequestTask = NewObject<UTwitchChatConnector>();
	RequestTask->OnMessage = OnMessage;
	RequestTask->OnCheer = OnCheer;

	// Execute the new class' start function so it can create it's async task/set itself up
	RequestTask->Start(Credentials, Channel);

	// Return the new instance so the blueprint OnSuccess/OnFail delegates actually work
	return RequestTask;
}

void UTwitchChatConnector::BeginDestroy()
{
	// Is the async task still running?
	if (bIsChatActive && AsyncWorkTask && AsyncWorkTask->bIsTaskActive)
	{
		// Leave the IRC channel and stop the task
		AsyncWorkTask->SendOverSocket(FString::Printf(TEXT("PART #%s"), *Channel));
		AsyncWorkTask->StopTask(); // Stop the work in the task so it can gracefully abandon + stop listening
	}

	// Call the "standard" BeginDestroy behavior
	Super::BeginDestroy();
}

void UTwitchChatConnector::Start(FTwitchLoginCredentials Credentials, FString _Channel)
{
	// Save the passed in details/credentials in variables inside of the class.
	Channel = _Channel;
	ConnectedUserCreds = Credentials;

	// Force-init the chat mode to ensure it isn't invalid when we try to use it later
	ChatMode = FTwitchChatMode();

	// Create/bind callback delegates
	FChatFeedCallbackDelegate onMessageDelegate;
	onMessageDelegate.BindUObject(this, &UTwitchChatConnector::TwitchIRCMessageRecieved);

	FReferenceCallbackDelegate referenceDelegate;
	referenceDelegate.BindUObject(this, &UTwitchChatConnector::ReferenceCallbackFunction);

	FReferenceCallbackDelegate connectionFailureDelegate;
	connectionFailureDelegate.BindUObject(this, &UTwitchChatConnector::OnChatConnectionFailed);

	// Create an async task that will handle the socket connection
	FAutoDeleteAsyncTask<FTwitchChatConnectorTask>* asyncTask = (new FAutoDeleteAsyncTask<FTwitchChatConnectorTask>(onMessageDelegate, referenceDelegate, this, Credentials, _Channel, connectionFailureDelegate));
	asyncTask->StartBackgroundTask();
}

void UTwitchChatConnector::TwitchIRCMessageRecieved(FString Message)
{
	// Attempt to parse the message
	FString Buffer = Message;
	if (Message.Contains(TEXT("GLOBALUSERSTATE")))
	{
		// Successful login - returns info about the user

		// Tokenize/parse the message
		FString FormatString = TEXT("@%s :tmi.twitch.tv GLOBALUSERSTATE");
		TArray<FString> parsedContent = ParseTwitchMessage(Buffer, FormatString);

		// Was the parse successful?
		if (parsedContent.Num() <= 0)
		{
			UE_LOG(LogTwitchWorks, Error, TEXT("Unable to interpret GLOBALUSERSTATE \"packet\"! Packet: %s"), *Buffer);
			return;
		}

		// Extract the tags out of the first part of the parsed content (up to the first space)
		TMap<FString, FString> tags = ParseTwitchTags(parsedContent[0]);

		// Build a TwitchUser out of the tags
		FTwitchUser TwitchUser;

		// Color
		TwitchUser.Color = tags.Contains("color") && tags["color"].Len() > 0 ? FColor::FromHex(tags["color"]) : FColor(); // TODO think of a better way to signal no custom color
		// Display name
		TwitchUser.DisplayName = tags.Contains("display-name") ? tags["display-name"] : TEXT(""); // TODO not always set, so add username field too?
		// Is this user a turbo subscriber?
		TwitchUser.bIsTurbo = tags.Contains("turbo") && tags["turbo"] == TEXT("1");
		// What's the user's ID?
		TwitchUser.UserId = tags.Contains("user-id") ? tags["user-id"] : TEXT("");
		// Emote sets
		TwitchUser.EmoteSets = GetEmoteSets(tags);

		// Does the user have any special permissions? (staff, mod, etc.)
		FString UserTypeString = tags.Contains("user-type") ? tags["user-type"] : TEXT("empty");

		// Try to match it to the correct enum, highest type to lowest type
		if (UserTypeString == TEXT("staff"))
		{
			TwitchUser.UserType = ETwitchUserType::TUT_STAFF;
		}
		else if (UserTypeString == TEXT("admin"))
		{
			TwitchUser.UserType = ETwitchUserType::TUT_ADMIN;
		}
		else if (UserTypeString == TEXT("global_mod"))
		{
			TwitchUser.UserType = ETwitchUserType::TUT_GLOBALMOD;
		}
		else if (UserTypeString == TEXT("mod"))
		{
			TwitchUser.UserType = ETwitchUserType::TUT_MOD;
		}
		else // empty | default
		{
			TwitchUser.UserType = ETwitchUserType::TUT_EMPTY;
		}


		// We successfully logged in and managed to get some info about the twitch user, call the successful login delegate in the game thread to avoid threading issues in blueprint
		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			OnSuccessfulLogin.Broadcast(TwitchUser, this); // TODO broadcast login fail when the socket disconnects?
		});

	}
	else if (Message.Contains(TEXT("PRIVMSG")) || Message.Contains(TEXT("WHISPER"))) // Probably a new message
	{
		// Are there any tags in the twitch message? If so account for that while parsing
		FString FormatString = TEXT("");
		// NOTE1: We're combining whisper and "regular" chat message parsing as they are almost the same, just with a few subtle differences in how they should be parsed/how the sending instance is stored.
		//        as such, it makes more sense to combine them than to separate them.
		// NOTE2: The not contains PRIVMSG check is in here to ensure we don't accidentally trigger when someone "shouts" WHISPER (in all caps) in their message
		bool bIsWhisper = Message.Contains(TEXT("WHISPER")) && !Message.Contains(TEXT("PRIVMSG"));
		if (Buffer.Len() > 0 && Buffer.Mid(0, 1) == TEXT("@"))
		{
			// NOTE: Using ternary operator to avoid having to deal with FString::Printf a %s that aren't to be used in formatting
			FormatString = bIsWhisper ? TEXT("@%s :%s!%s@%s.tmi.twitch.tv WHISPER %s :%s") : TEXT("@%s :%s!%s@%s.tmi.twitch.tv PRIVMSG #%s :%s"); // Message has tags
		}
		else
		{
			FormatString = bIsWhisper ? TEXT(":%s!%s@%s.tmi.twitch.tv WHISPER %s :%s") : TEXT(":%s!%s@%s.tmi.twitch.tv PRIVMSG #%s :%s"); // Message has tags
		}

		// Parse the twitch using the correct format string (with or without tags)
		TArray<FString> parsedContent = ParseTwitchMessage(Buffer, FormatString);

		// If there where indeed tags, extract them and remove the tags string from the array
		TMap<FString, FString> tags;
		if (parsedContent.Num() > 0 && Buffer.Len() > 0 && Buffer.Mid(0, 1) == TEXT("@"))
		{
			tags = ParseTwitchTags(parsedContent[0]);
			parsedContent.RemoveAt(0);
		}

		// Build a Twitch message author struct around the tags (if they exist, use defaults otherwise) and the IRC command
		FTwitchMessageAuthor MessageAuthor;

		// What is the author's username?
		MessageAuthor.Name = parsedContent[0]; // TODO are all 3 usersnames the same?
		// What's the author's display name?
		MessageAuthor.DisplayName = tags.Contains("display-name") ? tags["display-name"] : TEXT("");
		// What's the author's chat color (only set if the user has used the /color chat command)
		MessageAuthor.Color = tags.Contains("color") && tags["color"].Len() > 0 ? FColor::FromHex(tags["color"]) : FColor();
		// Is the author a channel moderator?
		MessageAuthor.bIsMod = tags.Contains("mod") && tags["mod"] == TEXT("1");
		// Is the author a channel subscriber?
		MessageAuthor.bIsSubscriber = tags.Contains("subscriber") && tags["subscriber"] == TEXT("1");
		// Does the author have twitch turbo?
		MessageAuthor.bIsTurbo = tags.Contains("turbo") && tags["turbo"] == TEXT("1");
		// What's the author's user id? @note not an actual user id, more like a user type
		MessageAuthor.UserId = tags.Contains("user-id") ? tags["user-id"] : TEXT("");

		// Is the user "special"? (eg. twitch staff, admin, etc.)
		FString UserTypeString = tags.Contains("user-type") ? tags["user-type"] : TEXT("empty");
		// Try to match it to the correct enum, highest type to lowest type
		if (UserTypeString == TEXT("staff"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_STAFF;
		}
		else if (UserTypeString == TEXT("admin"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_ADMIN;
		}
		else if (UserTypeString == TEXT("global_mod"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_GLOBALMOD;
		}
		else if (UserTypeString == TEXT("mod"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_MOD;
		}
		else // empty | default
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_EMPTY;
		}

		// Attempt to extract the author's badges out of the tags map
		MessageAuthor.Badges = GetTwitchBadges(tags);

		// Create a twitch message based upon the parsed data/previously created message author
		FTwitchMessage TwitchMessage;
		TwitchMessage.Author = MessageAuthor;

		// parsedContent indexes (for a successful parse): 0,1,2 -> usernames; 3 -> channel 4 -> body
		TwitchMessage.MessageBody = parsedContent.Num() >= 5 ? parsedContent[4] : TEXT("");
		TwitchMessage.MessageId = tags.Contains("msg-id") ? tags["msg-id"] : TEXT("");
		TwitchMessage.RoomId = tags.Contains("room-id") ? tags["room-id"] : TEXT("");

		// Parse the emotes (if any)
		if (tags.Contains(TEXT("emotes")))
		{
			TwitchMessage.Emotes = ParseEmotesFromMessage(TwitchMessage.MessageBody, tags["emotes"]);
		}
		
		// Was this message a cheer?
		if (tags.Contains("bits"))
		{
			// Yes, add the bits metadata to the message
			FTwitchCheer TwitchCheer;
			TwitchCheer.Message = TwitchMessage;
			TwitchCheer.Bits = FCString::Atoi(*tags["bits"]);

			UE_LOG(LogTwitchWorks, Verbose, TEXT("Cheer Message! Buffer: %s"), *Buffer);

			// Make sure to call the OnCheer delegate on the game thread so breakpoints etc. don't mess up.
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				if (!OnCheer.IsBound())
					UE_LOG(LogTwitchWorks, Error, TEXT("O.o OnCheer Delegate Not Bound!"))
				else
					OnCheer.ExecuteIfBound(TwitchCheer);
			});
		}
		// Was this a channel points event message?
		else if (tags.Contains("custom-reward-id"))
		{
			// Yes, add the bits metadata to the message
			FTwitchPointsEvent PointsEvent;
			PointsEvent.Message = TwitchMessage;
			PointsEvent.RewardID = tags["custom-reward-id"];

			UE_LOG(LogTwitchWorks, Verbose, TEXT("Channel Points Reward Message! Buffer: %s"), *Buffer);

			// Make sure to call the OnPointsEvent delegate on the game thread so breakpoints etc. don't mess up.
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				if (!OnPointsEvent.IsBound())
					UE_LOG(LogTwitchWorks, Error, TEXT("O.o OnPointsEvent Delegate Not Bound!"))
				else
					OnPointsEvent.ExecuteIfBound(PointsEvent);
			});
		}
		else if (bIsWhisper)
		{
			UE_LOG(LogTwitchWorks, Warning, TEXT("Whisper received!"))
			AsyncTask(ENamedThreads::GameThread, [=]()
				{
					if (!OnWhisperEvent.IsBound())
						UE_LOG(LogTwitchWorks, Error, TEXT("O.o OnWhisperEvent Delegate Not Bound!"))
					else
						OnWhisperEvent.ExecuteIfBound(TwitchMessage);
				});
		}
		else
		{
			// We received a message and successfully parsed it! Pass it onto the next step (command checking/delegate calling)
			OnTwitchMessageRecieved(TwitchMessage);
		}
	}
	else if (Message.Contains(TEXT("USERNOTICE"))) // Likely a new subscription/subscription renewal, or raid/channel ritual
	{
		// How should the chat message be parsed? (based on the twitch documentation, they hould always contain tags)
		FString FormatString = TEXT("@%s :tmi.twitch.tv USERNOTICE #%s :%s");

		// Parse the twitch using the correct format string (with or without tags)
		TArray<FString> parsedContent = ParseTwitchMessage(Buffer, FormatString);

		// If there where indeed tags, extract them and remove the tags string from the array
		TMap<FString, FString> tags;
		if (parsedContent.Num() > 0 && Buffer.Len() > 0 && Buffer.Mid(0, 1) == TEXT("@"))
		{
			tags = ParseTwitchTags(parsedContent[0]);
			parsedContent.RemoveAt(0);
		}

		// Build a Twitch message author struct around the tags (if they exist, use defaults otherwise) and the IRC command
		FTwitchMessageAuthor MessageAuthor;

		// What is the author's username?
		MessageAuthor.Name = tags.Contains("login") ? tags["login"] : TEXT("");
		// What's the user's display name? (can deviate from the username)
		MessageAuthor.DisplayName = tags.Contains("display-name") ? tags["display-name"] : TEXT("");
		// What's the author's chat color (only set if the user has used the /color chat command)
		MessageAuthor.Color = tags.Contains("color") && tags["color"].Len() > 0 ? FColor::FromHex(tags["color"]) : FColor();
		// Is the author a channel moderator?
		MessageAuthor.bIsMod = tags.Contains("mod") && tags["mod"] == TEXT("1");
		// Is the author a channel subscriber?
		MessageAuthor.bIsSubscriber = tags.Contains("subscriber") && tags["subscriber"] == TEXT("1");
		// Does the author have twitch turbo?
		MessageAuthor.bIsTurbo = tags.Contains("turbo") && tags["turbo"] == TEXT("1");
		// What's the author's user id? @note not an actual user id, more like a user type
		MessageAuthor.UserId = tags.Contains("user-id") ? tags["user-id"] : TEXT("");

		// Is the user "special"? (eg. twitch staff, admin, etc.)
		FString UserTypeString = tags.Contains("user-type") ? tags["user-type"] : TEXT("empty");
		// Try to match it to the correct enum, highest type to lowest type
		if (UserTypeString == TEXT("staff"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_STAFF;
		}
		else if (UserTypeString == TEXT("admin"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_ADMIN;
		}
		else if (UserTypeString == TEXT("global_mod"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_GLOBALMOD;
		}
		else if (UserTypeString == TEXT("mod"))
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_MOD;
		}
		else // empty | default
		{
			MessageAuthor.UserType = ETwitchUserType::TUT_EMPTY;
		}

		// Attempt to extract the author's badges out of the tags map
		MessageAuthor.Badges = GetTwitchBadges(tags);

		// Create a twitch message based upon the parsed data/previously created message author
		FTwitchMessage TwitchMessage;
		TwitchMessage.Author = MessageAuthor;

		// parsedContent indexes (for a successful parse): 0,1,2 -> usernames; 3 -> channel 4 -> body
		TwitchMessage.MessageBody = parsedContent.Num() >= 5 ? parsedContent[4] : TEXT("");
		TwitchMessage.RoomId = tags.Contains("room-id") ? tags["room-id"] : TEXT("");

		// Now that we've extracted the sender and user data, we need to check what kind of event/USERNOTICE happened.
		FString MessageID = tags.Contains("msg-id") ? tags["msg-id"] : TEXT("");

		if (MessageID == TEXT("sub") || MessageID == TEXT("resub") || MessageID == TEXT("subgift")
			|| MessageID == TEXT("anonsubgift") || MessageID == TEXT("submysterygift") || MessageID == TEXT("giftpaidupgrade")
			|| MessageID == TEXT("rewardgift") || MessageID == TEXT("anongiftpaidupgrade"))
		{
			// Create a new twitch subscription struct, but we can't (easily) initialize it from the parent, so manually init it by copying.
			FTwitchSubscription TwitchSubscription;
			TwitchSubscription.Author = TwitchMessage.Author;
			TwitchSubscription.RoomId = TwitchMessage.RoomId;
			TwitchSubscription.MessageBody = TwitchMessage.MessageBody;
			TwitchSubscription.SubGiftSize = -1; // Set to -1 default

			// Set properties that are under the same name for both "regular" and gifted subs
			FString _SubPlan = tags.Contains("msg-param-sub-plan") ? tags["msg-param-sub-plan"] : TEXT(""); 
			ETwitchSubscriptionType SubPlan;
			if (_SubPlan == TEXT("Prime"))
			{
				SubPlan = ETwitchSubscriptionType::TST_PRIME;
			}
			else if (_SubPlan == TEXT("1000"))
			{
				SubPlan = ETwitchSubscriptionType::TST_First;
			}
			else if (_SubPlan == TEXT("2000"))
			{
				SubPlan = ETwitchSubscriptionType::TST_Second;
			}
			else if (_SubPlan == TEXT("3000"))
			{
				SubPlan = ETwitchSubscriptionType::TST_Third;
			}
			else
			{
				SubPlan = ETwitchSubscriptionType::TST_UNKNOWN;
			}
			TwitchSubscription.SubscriptionType = SubPlan;



			if (MessageID == TEXT("subgift") || MessageID == TEXT("anonsubgift"))
			{
				TwitchSubscription.bWasGift = true;


				TwitchSubscription.NumMonthsSubscribed = tags.Contains("msg-param-months") ? FCString::Atoi(*tags["msg-param-months"]) : -1;
				TwitchSubscription.GiftRecipientUsername = tags.Contains("msg-param-recipient-display-name") ? tags["msg-param-recipient-display-name"] : TEXT("");
			}
			else if (MessageID == TEXT("giftpaidupgrade") || MessageID == TEXT("anongiftpaidupgrade"))
			{
				TwitchSubscription.bWasGift = true;
				UE_LOG(LogTwitchWorks, Log, TEXT("Received giftpaidupgrade or anongiftpaidupgrade subscription event! Raw Message: %s"), *Message)
			}
			else if (MessageID == TEXT("submysterygift"))
			{
				TwitchSubscription.SubGiftSize = tags.Contains("msg-param-mass-gift-count") ? FCString::Atoi(*tags["msg-param-mass-gift-count"]) : -1;
			}
			else if (MessageID == TEXT("rewardgift"))
			{
				TwitchSubscription.bWasGift = true;
				UE_LOG(LogTwitchWorks, Log, TEXT("Received rewardgift subscription event! Raw Message: %s"), *Message)
			}
			else
			{
				TwitchSubscription.bWasNewSub = MessageID == TEXT("sub");
				TwitchSubscription.bWasGift = false;
				TwitchSubscription.GiftRecipientUsername = TEXT("");
				TwitchSubscription.bWasResub = MessageID == TEXT("resub");

				TwitchSubscription.NumMonthsSubscribed = tags.Contains("msg-param-cumulative-months") ? FCString::Atoi(*tags["msg-param-cumulative-months"]) : -1;
			}

			// Execute the delegate so some blueprint code can execute, and then exit this code.
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnNewSubscription.ExecuteIfBound(TwitchSubscription);
			});

			return;
		}
		else if (MessageID == TEXT("ritual"))
		{
			// Currently only msg-param-ritual-name, but keeping it as a string for future compatibility.
			FString RitualName = tags.Contains("msg-param-ritual-name") ? tags["msg-param-ritual-name"] : TEXT("TWITCHWORKS_ERROR_RETUAL_NAME_NOT_SET");

			// Execute the delegate so some blueprint code can execute, and then exit this code.
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnRitual.ExecuteIfBound(TwitchMessage, RitualName);
			});

			return;
		}
		else if (MessageID == TEXT("raid"))
		{
			// Notification of an incoming raid from another channel.

			// Extract the raid info from the USERNOTICE
			FTwitchRaidEvent RaidEvent;
			RaidEvent.RaiderDisplayName = tags.Contains("msg-param-displayName") ? tags["msg-param-displayName"] : TEXT("");
			RaidEvent.RaiderUsername = tags.Contains("msg-param-login") ? tags["msg-param-login"] : TEXT("");
			RaidEvent.NumRaiders = tags.Contains("msg-param-viewerCount") ? FCString::Atoi(*tags["msg-param-viewerCount"]) : -1;

			// Execute the delegate so some blueprint code can execute, and then exit this code (no need to process anything else).
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				OnRaidEvent.ExecuteIfBound(RaidEvent);
			});

			return;
		}
		else
		{
			UE_LOG(LogTwitchWorks, Verbose, TEXT("Unable to understand twitch IRC USERNOTICE message/command: %s"), *Message);
			return;
		}
	}
	else if (Message.Contains(TEXT("NOTICE")))
	{
		// Did user authentication fail? (likely an expired token
		if (Message.Contains(TEXT(":tmi.twitch.tv NOTICE * :Login authentication failed")))
		{
			UE_LOG(LogTwitchWorks, Warning, TEXT("Login authentication failed! Your OAuth token has likely expired: please log in again."))

			// Execute the delegate so some blueprint code can execute, and then exit this code.
			AsyncTask(ENamedThreads::GameThread, [=]()
			{
				if (OnFailedLogin.IsBound())
				{
					OnFailedLogin.Broadcast(FTwitchUser(), nullptr);
				}
			});

			return;
		}
		else
		{
			UE_LOG(LogTwitchWorks, Verbose, TEXT("Unable to understand twitch IRC NOTICE message/command: %s"), *Message);
			return;
		}
	}
	else if (Message.Contains(TEXT("ROOMSTATE")))
	{
		// This is a ROOMSTATE update: it contains information/updates about which chat modes are enabled and/or disabled.

		// Parse the Twitch tags
		FString FormatString = TEXT("@%s :tmi.twitch.tv ROOMSTATE #%s :%s");
		TArray<FString> parsedContent = ParseTwitchMessage(Buffer, FormatString);
		TMap<FString, FString> tags;
		if (parsedContent.Num() > 0 && Buffer.Len() > 0 && Buffer.Mid(0, 1) == TEXT("@"))
		{
			tags = ParseTwitchTags(parsedContent[0]);
			parsedContent.RemoveAt(0);
		}

		// Update the chat mode so other functions can access the info
		// NOTE: Using ifs rather than a && to avoid overwriting it with an off value if an update for another property comes in.
		if (tags.Contains("emote-only"))
		{
			ChatMode.bEmoteOnly = tags["emote-only"] == TEXT("1");
		}
		if (tags.Contains("followers-only"))
		{
			ChatMode.bFollowersOnly = tags["followers-only"] == TEXT("1");
		}
		if (tags.Contains("r9k"))
		{
			ChatMode.bR9KEnabled = tags["r9k"] == TEXT("1");
		}
		if (tags.Contains("subs-only"))
		{
			ChatMode.bSubOnly = tags["subs-only"] == TEXT("1");
		}
		if (tags.Contains("slow"))
		{
			ChatMode.SlowTime = FCString::Atof(*tags["slow"]);
		}
	}
	else if (Message.Contains(TEXT("CAP * ACK")))
	{
		// Caphability acknowledgements from the login procedure - nothign to do with these, but we don't want to log an unable to parse error so we just do nothing
	}
	else if(Message.Contains(" USERSTATE ") || Message.Contains(TEXT(" JOIN ")))
	{
		// We also don't currently care about USERSTATE or JOIN updates (with spaces to avoid triggering for ones where it's a part of another word such as 'GLOBALUSERSTATE')
	}
	else
	{
		// If we got here that means something went wrong, log a warning showing the IRC command's body
		UE_LOG(LogTwitchWorks, Verbose, TEXT("Unable to understand twitch IRC message/command: %s"), *Message)
	}
}

TArray<FString> UTwitchChatConnector::ParseTwitchMessage(FString Buffer, FString FormatString)
{
	// Set up buffers/parsing variables
	FString formatCache = TEXT("");
	FString bufferCache = TEXT("");
	TArray<FString> parsedContent;
	int currentType = 0;
	int formatIndex = 0;
	bool bStartedCollecting = false;

	// Avoid first letter being cut off by adding a fake letter
	Buffer = TEXT("*") + Buffer;

	// "Parse" the buffer
	for (int i = 0; i < Buffer.Len(); i++)
	{
		// Create a temporary letter buffer to make it easier to work with.
		FString letter = Buffer.Mid(i, 1);

		// Does the letter match the required format character to move to the next "section"?
		if ((formatCache.Len() > 0 && formatCache == letter) || formatCache.Len() == 0)
		{
			// Is there anything in the buffer cache? If so, add it to the parsed content array
			if (bufferCache.Len() > 0)
			{
				// Process Buffer!
				if (bStartedCollecting)
				{
					parsedContent.Add(bufferCache);
				}

				bufferCache = TEXT("");
			}
format_goto:
			if (formatIndex < FormatString.Len() && FormatString.Mid(formatIndex, 1) == TEXT("%"))
			{
				bStartedCollecting = true;
				formatIndex++;
				if (formatIndex < FormatString.Len())
				{
					formatCache = TEXT("%") + formatCache.Mid(formatIndex, 1);
					formatIndex++;
					if (formatCache == TEXT("%s"))
					{
						currentType = 1;
					}
					else if (formatCache == "%i")
					{
						currentType = 2;
					}
					if (formatIndex < FormatString.Len()) // Is there buffer left?
						goto format_goto;
					else // This is the last character, assign a bogos formatCache so it won't trigger again & simply follow this formatCache operator until the end
					{
						formatCache = TEXT("THEEND");
					}
				}
			}
			// Would the formatIndex be out of range if we incrememnted it?
			else if (formatIndex < FormatString.Len())
			{
				formatCache = FormatString.Mid(formatIndex, 1);
				formatIndex++;
			}
			// Avoids annoying issues where it is unable to parse when the string doesn't end with a %type
			else if (formatIndex == FormatString.Len())
			{
				formatCache = TEXT("THEEND");
			}
			else
			{
				// Out of index, must be invalid parse!
				UE_LOG(LogTwitchWorks, Warning, TEXT("Unable to parse string!"))
				return TArray<FString>();
			}
		}
		else
		{
			// If this isn't the next delimiter, then append the letter to the cache
			bufferCache += letter;
		}
	}

	// Any bufferCache left after loop? (caused by having a format character as the last delimiter)
	if (bufferCache.Len() > 0)
	{
		// If so, remove the newline at the end and add it to the parsed content array
		if (!bufferCache.RemoveFromEnd(TEXT("\r\n"))) // Carrage return (windows)
			bufferCache.RemoveFromEnd(TEXT("\n"));	// Newline (other (unix?) OSes)

		parsedContent.Add(bufferCache);
		bufferCache = TEXT("");
	}

	return parsedContent;
}

TMap<FString, FString> UTwitchChatConnector::ParseTwitchTags(FString Buffer)
{
	// Create local variables to hold the tags that where already parsed & a buffer to save the current tag
	TMap<FString, FString> tags;
	FString tagBuffer = TEXT("");

	// Are there even any tags that we can get out of the string?
	if (Buffer.Len() > 0)
	{
		// Itterate over the buffer, letter by letter, trying to find the tag delimiter (;) and then starting a new tag.
		for (int i = 0; i <= Buffer.Len(); i++)
		{
			// Is this a tag delimiter (or the end of the string)?
			if (Buffer.Mid(i, 1) == TEXT(";") || i == Buffer.Len())
			{
				// Did we actually parse anything?
				if (tagBuffer.Len() > 0)
				{
					// Attempt to split the key & value (delimiter is an equals sign)
					FString tagKey = TEXT("");
					FString tagValue = TEXT("");
					tagBuffer.Split(TEXT("="), &tagKey, &tagValue);

					tags.Add(tagKey, tagValue);
					tagBuffer = TEXT("");
				}
			}
			else
			{
				// Add the current character to the current tag's buffer
				tagBuffer.Append(Buffer.Mid(i, 1));
			}
		}
	}

	return tags;
}

TMap<FIntPoint, FTwitchEmoticon> UTwitchChatConnector::ParseEmotesFromMessage(FString MessageBody, FString EmotesTag)
{
	// According to https://dev.twitch.tv/docs/irc/tags#privmsg-twitch-tags, this will be either empty or in the following format:
	// <emote ID>:<first index>-<last index>,<another first index>-<another last index>/<another emote ID>:<first index>-<last index>...

	// Declare a variable to store the results
	TMap<FIntPoint, FTwitchEmoticon> ParsedEmotes;

	// First split off all individual emotes
	TArray<FString> EmotesInMessage;
	EmotesTag.ParseIntoArray(EmotesInMessage, TEXT("/"));

	// Now obtain their code and all of the index "pairs"
	for (FString &EmoteSegment : EmotesInMessage)
	{
		FString EmoteId, EmoteLocationsString;
		const bool bSplitSuccess = EmoteSegment.Split(TEXT(":"), &EmoteId, &EmoteLocationsString);

		// Ensure the split succeeded
		if (!bSplitSuccess || EmoteId.IsEmpty() || EmoteLocationsString.IsEmpty())
		{
			UE_LOG(LogTwitchWorks, Warning, TEXT("Received malformed emote segment: %s"), *EmoteSegment);
			continue;
		}

		// Finally parse the locations into the points and "save" the result
		TArray<FString> EmoteLocations;
		EmoteLocationsString.ParseIntoArray(EmoteLocations, TEXT(","));
		for (auto &EmoteLocation : EmoteLocations)
		{
			FString StartIndexString, EndIndexString;
			EmoteLocation.Split(TEXT("-"), &StartIndexString, &EndIndexString);
			int StartIndex = FCString::Atoi(*StartIndexString);
			int EndIndex = FCString::Atoi(*EndIndexString);

			// Extract the text to which this emoticon is assigned (assumed to equal the "code")
			FString EmoteCode = MessageBody.Mid(StartIndex, EndIndex - StartIndex + 1);

			// Build an emoticon object so the user has access to both the ID and the "code" (avoids breaking caching)
			FTwitchEmoticon Emote;
			Emote.Id = EmoteId;
			Emote.Code = EmoteCode;
			Emote.EmoteSet = -1; // We can't really get this info straight from the chat message, so don't set it to avoid having to call emote APIs

			// NOTE: Assuming there won't be any collisions/there can't be two emotes in one place
			ParsedEmotes.Add(FIntPoint(StartIndex, EndIndex), Emote);
		}
		
	}

	// Finished parsing (and if we somehow failed one of the checks we'll simply return the empty map we started with)
	return ParsedEmotes;
}

void FTwitchChatConnectorTask::OnTwitchMessage(const FString& InMessage)
{
	// Parse along the newline to avoid having multiple messages squeezed into one packet -> that causing parsing error or dropped messages down the line
	TArray<FString> Messages;

	// NOTE: Using CRLF (as per the rfc1459 spec) we should not be parsing newlines within messages, but only newlines
	// for when multiple messages get squished together.
	InMessage.ParseIntoArray(Messages, TEXT("\r\n"), true);
	
	// Iterate over each IRC message seperately, and attempt to parse them.
	for (auto& Message : Messages)
	{
		// Log that allows us to see what's going on in log files
		UE_LOG(LogTwitchWorks, Verbose, TEXT("< %s"), *Message);
		
		// Is this a twitch PING request?
		if (Message == TEXT("PING :tmi.twitch.tv"))
		{
			// Reply with a PONG (the twitch API expects this, else we'll be disconnected)
			if (WebSocket.IsValid() && WebSocket->IsConnected())
			{
				WebSocket->Send(TEXT("PONG :tmi.twitch.tv"));
			}
			else
			{
				UE_LOG(LogTwitchWorks, Error, TEXT("Unable to respond to PING request since WS is either invalid or got disconnected - were you disconnected?"));
			}


		}
		else
		{
			// Call the Target's OnMessageRecieved function to process the message/command
			Target->TwitchIRCMessageRecieved(Message);
		}
	}
	
}

void FTwitchChatConnectorTask::OnTwitchConnected()
{
	UE_LOG(LogTwitchWorks, Log, TEXT("Successfully established connection to TMI server - performing login & CAP request..."));
	
	/*
	 *    Start of twitch capability requesting
	 */
	 // Get the tags capability to get meta data throughout the process
	{
		FString Message = TEXT("CAP REQ :twitch.tv/tags\n");
		WebSocket->Send(Message);
	}
	// Get the membership capability to get member information during the IRC process
	{
		FString Message = TEXT("CAP REQ :twitch.tv/membership\n");
		WebSocket->Send(Message);
	}
	// Get the commands capability so we can use twitch-specific IRC commands
	{
		FString Message = TEXT("CAP REQ :twitch.tv/commands\n");
		WebSocket->Send(Message);
	}
	/*
	*	End of twitch capability requesting
	*/

	/*
	*	Start of twitch oauth login
	*/
	// Send the twitch oauth token
	{
		FString Message = FString::Printf(TEXT("PASS oauth:%s\n"), *TwitchCredentials.OAuthToken);
		WebSocket->Send(Message);
	}
	// Send the twitch username to complete the oauth process
	{
		FString Message = FString::Printf(TEXT("NICK %s\n"), *TwitchCredentials.Username);
		WebSocket->Send(Message);
	}
	/*
	*	End of twitch caphability requesting
	*/

	// Attempt to join the specified channel
	{
		FString Message = FString::Printf(TEXT("JOIN #%s\n"), *Channel);
		WebSocket->Send(Message);
	}
}

void FTwitchChatConnectorTask::OnConnectionError(const FString& Error)
{
	UE_LOG(LogTemp, Error, TEXT("Encountered connection error while connecting to the Twitch TMI service. Error: %s"), *Error);
	OnFailure.ExecuteIfBound(nullptr);
}

void FTwitchChatConnectorTask::OnClosed(int StatusCode, const FString& Error, bool bWasClean)
{
	if (bWasClean)
	{
		UE_LOG(LogTemp, Log, TEXT("Clean Twitch connection closure. Goodbye!"));
	}
	else
	{
		// Connection close was not clean - something must have gone wrong
		UE_LOG(LogTemp, Error, TEXT("Encountered non-clean connection closure with Twitch TMI service. Code: %i, Error: %s"), StatusCode, *Error);
		OnFailure.ExecuteIfBound(nullptr);
	}
}

TArray<FTwitchBadge> UTwitchChatConnector::GetTwitchBadges(TMap<FString, FString> Tags)
{
	// Set up variables to store the parsing state & results
	bool bIsInBadgeVersion = false;
	FString BadgeName = TEXT("");
	FString BadgeVersion = TEXT("");
	TArray<FTwitchBadge> Badges;

	// Does the tags map actually contain a badges key/tag?
	FString BadgeBuffer = TEXT("");
	if (Tags.Contains("badges"))
	{
		BadgeBuffer = Tags["badges"];
	}
	else
	{
		return TArray<FTwitchBadge>();
	}

	// Extract the badges from the buffer assuming comma separated list of badges and a badge following "name/version"
	for (int i = 0; i < BadgeBuffer.Len(); i++)
	{
		// Create an FString buffer for the current letter so it's easier to refer to
		FString letter = BadgeBuffer.Mid(i, 1);

		// Is the current letter the new tag delimiter(",")?
		if (letter == TEXT(","))
		{
			// Next badge
			FTwitchBadge Badge;

			// Attempt to convert the badge's name into a badge enum. Log out an error if the badge was not found
			if (BadgeName == TEXT("admin"))
			{
				Badge.Badge = ETwitchBadge::TB_ADMIN;
			}
			else if (BadgeName == TEXT("bits"))
			{
				Badge.Badge = ETwitchBadge::TB_BITS;
			}
			else if (BadgeName == TEXT("broadcaster"))
			{
				Badge.Badge = ETwitchBadge::TB_BROADCASTER;
			}
			else if (BadgeName == TEXT("global_mod"))
			{
				Badge.Badge = ETwitchBadge::TB_GLOBALMOD;
			}
			else if (BadgeName == TEXT("moderator"))
			{
				Badge.Badge = ETwitchBadge::TB_MOD;
			}
			else if (BadgeName == TEXT("subscriber"))
			{
				Badge.Badge = ETwitchBadge::TB_SUBSCRIBER;
			}
			else if (BadgeName == TEXT("staff"))
			{
				Badge.Badge = ETwitchBadge::TB_STAFF;
			}
			else if (BadgeName == TEXT("turbo"))
			{
				Badge.Badge = ETwitchBadge::TB_TURBO;
			}
			else if (BadgeName == TEXT("partner"))
			{
				Badge.Badge = ETwitchBadge::TB_PARTNER;
			}
			else if (BadgeName == TEXT("premium"))
			{
				Badge.Badge = ETwitchBadge::TB_PREMIUM;
			}
			else if (BadgeName == TEXT("twitchbot"))
			{
				Badge.Badge = ETwitchBadge::TB_TWITCHBOT;
			}
			else if (BadgeName == TEXT("vip"))
			{
				Badge.Badge = ETwitchBadge::TB_VIP;
			}
			else if (BadgeName == TEXT("sub-gifter"))
			{
				Badge.Badge = ETwitchBadge::TB_SUBGIFTER;
			}
			else
			{
				// Unknown badge - user should still be able to identify it by the _RawBadgeCode_ set at the same time as the version is set.
				Badge.Badge = ETwitchBadge::TB_UNKNOWN;
			}

			// Set the badge's version & raw "identifier" and add it to the badges array
			Badge.Version = FCString::Atoi(*BadgeVersion);
			Badge.RawBadgeCode = BadgeName;
			Badges.Add(Badge);

			// Reset the parsing variables to avoid issues for people with multiple badges
			BadgeName = TEXT("");
			BadgeVersion = TEXT("");
			bIsInBadgeVersion = false;
		}
		// Is this letter the name -> version delimiter("/")?
		else if (letter == TEXT("/"))
		{
			bIsInBadgeVersion = true;
		}
		else
		{
			// Otherwise add it the the correct buffer.
			if (bIsInBadgeVersion)
				BadgeVersion.Append(letter);
			else
				BadgeName.Append(letter);
		}

	}

	// Is there still (part) of a badge left we need to (try and) parse?
	if (BadgeName.Len() > 0)
	{
		FTwitchBadge Badge;

		// Attempt to convert the badge's name into a badge enum. Log out an error if the badge was not found
		if (BadgeName == TEXT("admin"))
		{
			Badge.Badge = ETwitchBadge::TB_ADMIN;
		}
		else if (BadgeName == TEXT("bits"))
		{
			Badge.Badge = ETwitchBadge::TB_BITS;
		}
		else if (BadgeName == TEXT("broadcaster"))
		{
			Badge.Badge = ETwitchBadge::TB_BROADCASTER;
		}
		else if (BadgeName == TEXT("global_mod"))
		{
			Badge.Badge = ETwitchBadge::TB_GLOBALMOD;
		}
		else if (BadgeName == TEXT("moderator"))
		{
			Badge.Badge = ETwitchBadge::TB_MOD;
		}
		else if (BadgeName == TEXT("subscriber"))
		{
			Badge.Badge = ETwitchBadge::TB_SUBSCRIBER;
		}
		else if (BadgeName == TEXT("staff"))
		{
			Badge.Badge = ETwitchBadge::TB_STAFF;
		}
		else if (BadgeName == TEXT("turbo"))
		{
			Badge.Badge = ETwitchBadge::TB_TURBO;
		}
		else if (BadgeName == TEXT("partner"))
		{
			Badge.Badge = ETwitchBadge::TB_PARTNER;
		}
		else if (BadgeName == TEXT("premium"))
		{
			Badge.Badge = ETwitchBadge::TB_PREMIUM;
		}
		else if (BadgeName == TEXT("twitchbot"))
		{
			Badge.Badge = ETwitchBadge::TB_TWITCHBOT;
		}
		else
		{
			// Unknown badge - user should still be able to identify it by the _RawBadgeCode_ set at the same time as the version is set.
			Badge.Badge = ETwitchBadge::TB_UNKNOWN;
		}

		// Set the badge's version & raw "identifier" and add it to the badges array
		Badge.Version = FCString::Atoi(*BadgeVersion);
		Badge.RawBadgeCode = BadgeName;
		Badges.Add(Badge);
	}

	return Badges;
}

TArray<int32> UTwitchChatConnector::GetEmoteSets(TMap<FString, FString> Tags)
{
	// Buffer to store a single emote set
	FString EmoteSetBuffer = TEXT("");
	// Buffer to store the (comma seperated) list of emote sets
	FString EmoteSetsBuffer = TEXT("");

	// The parsed array of emote set IDs
	TArray<int32> EmoteSets;
	
	if (Tags.Contains("emote-sets"))
	{
		EmoteSetsBuffer = Tags["emote-sets"];
	}
	else
	{
		return TArray<int32>();
	}

	// Extract the emote sets from the buffer assuming a comma separated list
	for (int i = 0; i < EmoteSetsBuffer.Len(); i++)
	{
		// Store the current letter as an FString to make it easiser to use
		FString Letter = EmoteSetsBuffer.Mid(i, 1);

		// Is the current letter the emote set delimiter (",")? 
		if (Letter == TEXT(","))
		{
			// Convert the emoteSetBuffer into an int, add it to the emote sets and reset the state to redo the parse
			EmoteSets.Add(FCString::Atoi(*EmoteSetBuffer));
			EmoteSetBuffer = TEXT("");
		}
		else
		{
			// Add the current letter to the current emote set's buffer
			EmoteSetBuffer.Append(Letter);
		}
	}

	if (!EmoteSetBuffer.IsEmpty())
	{
		// Convert the emoteSetBuffer into an int, add it to the emote sets and reset the state to redo the parse
		EmoteSets.Add(FCString::Atoi(*EmoteSetBuffer));
		EmoteSetBuffer = TEXT("");
	}

	return EmoteSets;
}

void UTwitchChatConnector::ReferenceCallbackFunction(FTwitchChatConnectorTask *Task)
{
	// Sets the AsyncWorkTask variable to the correct instance
	AsyncWorkTask = Task;
}


void UTwitchChatConnector::OnChatConnectionFailed(FTwitchChatConnectorTask* Task)
{
	// Inform the BP caller about the failure
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			if (OnFailedLogin.IsBound())
			{
				OnFailedLogin.Broadcast(FTwitchUser(), nullptr);
			}
		});
}

void UTwitchChatConnector::OnTwitchMessageRecieved(FTwitchMessage TwitchMessage)
{
	// Does this message even qualify for being parsed for possibly being a command?
	if (Commands.Num() > 0 && TwitchMessage.MessageBody.Len() > GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix.Len() && TwitchMessage.MessageBody.Mid(0, GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix.Len()) == GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix)
	{
		// Go over all of the registered/bound commands
		for (FTwitchCommandBinding Command : Commands)
		{
			// Is this the correct command? (Can't use startswith because a command may be the same as another command starts with + finding the command name up to the space will work as a space can't be the first character. Also note that a command without parameters may miss a space, so we need to account for that.)
			if (TwitchMessage.MessageBody.Mid(0, TwitchMessage.MessageBody.Contains(TEXT(" ")) ? TwitchMessage.MessageBody.Find(TEXT(" ")) : /* Assume that if there isn't a space in the command string, the only thing it contains is the command name, so check the whole string */ TwitchMessage.MessageBody.Len()) == GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + Command.Command.CommandName)
			{
				FString commandBuffer = TwitchMessage.MessageBody;

				if (commandBuffer.StartsWith(GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + Command.Command.CommandName + TEXT(" ")))
				{
					// Remove the command's prefix, name + the name/command's tailing space character.
					commandBuffer.ReplaceInline(*(GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + Command.Command.CommandName + TEXT(" ") /* Avoid the space after the command being triggered as a token*/), TEXT(""));
				}
				else
				{
					// Remove the command's prefix + name.
					commandBuffer.ReplaceInline(*(GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + Command.Command.CommandName), TEXT(""));
				}

				// Extract the parameters out of the command
				TArray<FString> Parameters = ParseTwitchCommand(commandBuffer, Command.Command);

				// Did the sender provide the correct amount of parameters?
				if (Parameters.Num() == Command.Command.Parameters.Num())
				{
					// Execute the command's delegate and return to avoid also triggering any other delegates.
					AsyncTask(ENamedThreads::GameThread, [=]()
					{
						Command.Binding.ExecuteIfBound(Parameters, TwitchMessage.Author);
					});

					return;
				}
				else
				{
					// User used an invalid amount of parameters!
					UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid command usage!"));
					// Fall through to the message part. TODO is this indeed wanted behavior? [AWAITING CUSTOMER FEEDBACK]
					if (ensure(AsyncWorkTask))
					{
						// Send the message over the socket using the IRC standard
						AsyncWorkTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :%s\n"), *(Channel), *Command.Command.Usage.ToString()));
						return;
					}
				}
			}

		}

		// Is the help command enabled and is the user executing the help command?
		/// NOTE: The command can be called without a paremter (will list available commands) or with a parameter (the command name; will give the full description + parameters) - main reason for this all is the character limit (most likely 500 characters?) twitch imposes on the chat.
		if (GetMutableDefault<UTwitchRuntimeSettings>()->bIsCommandHelpEnabled && TwitchMessage.MessageBody.Mid(0, TwitchMessage.MessageBody.Contains(TEXT(" ")) ? TwitchMessage.MessageBody.Find(TEXT(" ")) : TwitchMessage.MessageBody.Len()) == FString::Printf(TEXT("%s%s"), *GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix, *GetMutableDefault<UTwitchRuntimeSettings>()->helpCommand))
		{
			FString commandBuffer = TwitchMessage.MessageBody;

			bool bWasCalledWithParameters;

			// Assume it's being called with parameters if there is a trailing space, and remove it for further parsing.
			if (commandBuffer.StartsWith(GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + GetMutableDefault<UTwitchRuntimeSettings>()->helpCommand + TEXT(" ")))
			{
				// Remove the command's prefix, name + the name/command's tailing space character.
				commandBuffer.ReplaceInline(*(GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + GetMutableDefault<UTwitchRuntimeSettings>()->helpCommand + TEXT(" ") /* Avoid the space after the command being triggered as a token*/), TEXT(""));
				bWasCalledWithParameters = true;
			}
			else
			{
				// Remove the command's prefix + name.
				commandBuffer.ReplaceInline(*(GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix + GetMutableDefault<UTwitchRuntimeSettings>()->helpCommand), TEXT(""));
				bWasCalledWithParameters = false;
			}

			if (bWasCalledWithParameters)
			{
				// Create a "bare" command so we can parse the command using the ParseTwitchCommand() function.
				FTwitchCommand HelpCommand_TC;
				HelpCommand_TC.CommandName = TEXT("help");
				FTwitchCommandParameter HelpCommand_TCParameter;
				HelpCommand_TCParameter.ParameterDelimiter = " ";
				HelpCommand_TC.Parameters.Add(HelpCommand_TCParameter);

				// Extract the parameters out of the command
				TArray<FString> Parameters = ParseTwitchCommand(commandBuffer, HelpCommand_TC);

				if (Parameters.Num() != 1)
				{
					// Incorrect # of parameters! Use the "regular" command instead
					goto PARSE_HELP_WITHOUT_PARAMETERS;
				}

				// Create a more detailed bit of help for the command
				FString helpTextBuffer = TEXT("");

				bool bWasCommandFound = false;

				// Go over all of the currently created/bound (AKA active) commands.
				for (FTwitchCommandBinding HelpCommandBinding : Commands)
				{
					// Fetch the actual command from the binding
					FTwitchCommand HelpCommand = HelpCommandBinding.Command;

					// Is this the command the user wants help with? (also check for the name with the command prefix)
					if (HelpCommand.CommandName == Parameters[0] || FString::Printf(TEXT("%s%s"), *GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix, *HelpCommand.CommandName) == Parameters[0])
					{
						// We found the command!
						bWasCommandFound = true;

						// Should this command be "hidden" or can it show up in the settings?
						if (HelpCommand.showsUpInHelp)
						{
							// Create a neatly formatted command buffer
							FString commnandBuffer = FString::Printf(TEXT("%s -> %s. Usage:  %s%s"), *HelpCommand.CommandName, *HelpCommand.CommandDescription.ToString(), *GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix, *HelpCommand.CommandName);

							// Add the parameters & their descriptions to the command buffer (if applicable)
							if (HelpCommand.Parameters.Num() > 0)
							{
								for (FTwitchCommandParameter Parameter : HelpCommand.Parameters)
								{
									commnandBuffer.Append(FString::Printf(TEXT(" <%s> (%s)"), *Parameter.ParameterName, *Parameter.ParameterDescription));
								}
							}
							else
							{
								// No parameters are available, so print something accordingly
								///commnandBuffer.Append(TEXT(" <NO PARAMETERS>"));
							}

							helpTextBuffer = commnandBuffer;
						}
						// Command doesn't show up in help/there isn't any help available
						else
						{
							helpTextBuffer = FString::Printf(TEXT("There is no help available for command %s!"), *HelpCommand.CommandName);
						}
					}
				}

				// Print out an "error" if the command wasn't found
				if (!bWasCommandFound)
				{
					helpTextBuffer = FString::Printf(TEXT("We were unable to find any help for the command %s! Are you sure you've spelled it correctly and it exists?"), *Parameters[0]);
				}

				// Attempt to send the help text message
				if (AsyncWorkTask)
				{
					AsyncWorkTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :%s\n"), *Channel, *helpTextBuffer));
				}
			}
			// If there weren't any parameters, then print out all of the avaialbe commands & an instruction on how to use the help command to get more info.
			else
			{
				// So we can skip here from the above called with parameters command
			PARSE_HELP_WITHOUT_PARAMETERS:

				// Automatically create a help message based upon the currently set up commands.
				FString helpTextBuffer = FString::Printf(TEXT("Available Commands (Use %s%s <command name> for more information on the command):"), *GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix, *GetMutableDefault<UTwitchRuntimeSettings>()->helpCommand);

				/*
				// Go over all of the commands swet up in the settings
				for (FTwitchCommand HelpCommand : GetMutableDefault<UTwitchRuntimeSettings>()->Commands)
				*/

				// Go over all of the currently created/bound (AKA active) commands that should show up in the help menu.
				for (FTwitchCommandBinding HelpCommandBinding : Commands)
				{
					// Fetch the actual command from the binding
					FTwitchCommand HelpCommand = HelpCommandBinding.Command;

					// Should this command be "hidden" or can it show up in the settings?
					if (HelpCommand.showsUpInHelp)
					{
						// Add the command's name to the buffer (not the description to allow larger games to not hit the character limit)
						FString commnandBuffer = FString::Printf(TEXT(" %s%s |"), *GetMutableDefault<UTwitchRuntimeSettings>()->commandPrefix, *HelpCommand.CommandName);

						helpTextBuffer.Append(commnandBuffer);
					}
				}

				// Attempt to send the help text message
				if (AsyncWorkTask)
				{
					AsyncWorkTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :%s\n"), *Channel, *helpTextBuffer));
				}
			}

			// The help command got executed - we don't need to call the message sent command
			return;
		}
	}

	// Execute the OnMessage delegate on the game thread to avoid any issues related to blueprint threading.
	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		if (!OnMessage.IsBound())
			UE_LOG(LogTwitchWorks, Error, TEXT("O.o OnMessage Delegate Not Bound!"))
		else
			OnMessage.ExecuteIfBound(TwitchMessage);
	});
}

TArray<FString> UTwitchChatConnector::ParseTwitchCommand(FString Buffer, FTwitchCommand Command)
{
	// Set up parameters/buffers
	TArray<FString> Parameters;
	int parameterIndex = 0;
	FString parameterBuffer = TEXT("");

	// Iterate over the buffer
	for (int i = 0; i < Buffer.Len(); i++)
	{
		// Store the current letter as an FString so we don't have to use .Mid() every time
		FString Letter = Buffer.Mid(i, 1);

		// Is this a valid parameter index? (eg. would the following if cause an out of bounds?)
		if (parameterIndex >= Command.Parameters.Num())
		{
			// Command may not be vaid, but return now to avoid any assertions/crashes.
			return Parameters;
		}

		// Is the character equal to the parameter's delimiter?
		if (Letter == Command.Parameters[parameterIndex].ParameterDelimiter)
		{
			// Found another parameter, add it to the array and reset
			Parameters.Add(FString(parameterBuffer));
			parameterBuffer = TEXT("");

			// Is it safe to go to the next parameter?
			if (parameterIndex + 1 < Command.Parameters.Num())
			{
				parameterIndex++;
			}
			else
			{
				// Invalid command usage! (too many parameters given)
				return TArray<FString>();
			}
		}
		else
		{
			// Add to parameter next parameter's buffer
			parameterBuffer.Append(Letter);
		}
	}
	
	// Is there anything left in the parameter buffer that we need to add?
	if (parameterBuffer.Len() > 0)
	{
		// Add the remaining parameter buffer to the parameters array
		Parameters.Add(FString(parameterBuffer));
	}

	return Parameters;
}

bool FTwitchChatConnectorTask::SendOverSocket(FString Message)
{
	if (!bIsTaskActive || !WebSocket || !WebSocket->IsConnected()) { return false; }

	// Verbose logging for debugging purposes
	UE_LOG(LogTwitchWorks, Verbose, TEXT("> %s"), *Message);
	WebSocket->Send(Message);
	return true;
}


void FTwitchChatConnectorTask::DoWork()
{
	// Connect to the Twitch TMI server over a secured WebSocket connection (should bypass firewalls more easily than IRC connections)
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(TEXT("wss://irc-ws.chat.twitch.tv"), TEXT(""));
	
	WebSocket->OnConnected().AddRaw(this, &FTwitchChatConnectorTask::OnTwitchConnected);
	WebSocket->OnMessage().AddRaw(this, &FTwitchChatConnectorTask::OnTwitchMessage);

	// Handle connection errors/closures so we can log info about them
	WebSocket->OnConnectionError().AddRaw(this, &FTwitchChatConnectorTask::OnConnectionError);
	WebSocket->OnClosed().AddRaw(this, &FTwitchChatConnectorTask::OnClosed);

	// Now that all of the relevant callbacks have been bound, we can attempt the connection to the socket.
	WebSocket->Connect();

	// NOTE: TwitchWorks's old system used IRC sockets, which required a task. We still need to keep the task active to avoid exiting -> that removing the enitre task -> callbacks being cleared as well, so instead just perform short delays until an exit is requested.
	while (bIsTaskActive)
	{
		// Wait for a new packet to arrive with a max. 
		while (bIsTaskActive)
		{
			if (bIsTaskActive)
			{
				FPlatformProcess::Sleep(0.1f);
			}
			else
			{
				// Task should no longer be active, so stop.
				goto StopTask;
			}
		}
	}

	// Stop the task, has StopTask: in front of it so we can call it from earlier in the code.
StopTask:
	UE_LOG(LogTwitchWorks, Warning, TEXT("TwitchWorks Chat Task Shutting Down..."));
	return;
}