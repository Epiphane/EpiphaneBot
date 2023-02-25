// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "TwitchPluginBPLibrary.h"
#include "TwitchPlugin.h"

UTwitchPluginBPLibrary::UTwitchPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool UTwitchPluginBPLibrary::SendTwitchMessage(UTwitchChatConnector *Chat, FString Message)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Send the message over the socket using the IRC standard
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :%s\n"), *(Chat->Channel), *Message));
	}

	// If we hit this that means something went wrong!
	return false;
}
bool UTwitchPluginBPLibrary::SendTwitchMessage(UTwitchChatConnector* Chat, FText Message)
{
	return SendTwitchMessage(Chat, Message.ToString());
}

bool UTwitchPluginBPLibrary::ParmaBanTwitchUser(UTwitchChatConnector *Chat, FString Username, FString Reason)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// NOTE: Recent changes to the way Twitch processes timeouts/bans mean that the CLEARCHAT command doesn't work the way it did.
		// To work around this, we'll simply issue the appropriate command (which should do exactly the same/means the users can keep on using the node without having to
		// build/format their own timeout commands)
		return SendTwitchMessage(Chat, FString::Printf(TEXT("/ban %s %s"), *Username, *Reason));
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::TempBanTwitchUser(UTwitchChatConnector *Chat, FString Username, int32 Duration, FString Reason)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// NOTE: Recent changes to the way Twitch processes timeouts/bans mean that the CLEARCHAT command doesn't work the way it did.
		// To work around this, we'll simply issue the appropriate command (which should do exactly the same/means the users can keep on using the node without having to
		// build/format their own timeout commands)
		return SendTwitchMessage(Chat, FString::Printf(TEXT("/timeout %s %i %s"), *Username, Duration, *Reason));
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::EnableSlowChat(UTwitchChatConnector *Chat, int32 Seconds)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the twitch slow <duration> command to enable slow chat.
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.slow %i\n"), *(Chat->Channel), Seconds)); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::DisableSlowChat(UTwitchChatConnector *Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the twitch slowoff command to disable slow chat.
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.slowoff\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::EnableEmoteChat(UTwitchChatConnector *Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the twitch emoteonly command to enable emote only chat.
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.emoteonly\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::DisableEmoteChat(UTwitchChatConnector *Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the twitch emoteonlyoff command to disable emote only chat
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.emoteonlyoff\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::EnableR9KChat(UTwitchChatConnector * Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Enable the r9k beta chat mode using r9kbeta. @TODO Update this when it is no longer beta!
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.r9kbeta\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::DisableR9KChat(UTwitchChatConnector * Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the .r9kbetaoff twitch command to disable the r9k chat mode. @TODO Update this when it is no longer beta!
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.r9kbetaoff\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::CearChat(UTwitchChatConnector * Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Send the twitch clear command to clear the chat.
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.clear\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::EnableFollowersOnlyChat(UTwitchChatConnector * Chat, FTimespan followDuration)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	FString followDurationString = TEXT("");

	// TODO FMath::Truct -> FMath::TructToInt

	// Turn an FTimespan into a format twich can understand
	if (followDuration.GetDays() / 30 >= 3.f) // Twitch supports a max of 3mo, assume 30d/mo
		// Hardcode a limit of 3 months
		followDurationString.Append("3mo");
	else
	{
		// Twitch supports stuff like 0h30m (30m) so no need to worry about inserting 0s. Assume 30d/mo
		followDurationString.Append(FString::FromInt(FMath::TruncToInt(followDuration.GetDays() / 30.f)) + TEXT("mo"));
		followDuration -= FTimespan(FMath::TruncToInt(followDuration.GetDays() / 30.f) * 30, 0, 0, 0); // Subtract the months

		// Assume 7d/w
		followDurationString.Append(FString::FromInt(FMath::TruncToInt(followDuration.GetDays() / 7.f)) + TEXT("w"));
		followDuration -= FTimespan(FMath::TruncToInt(followDuration.GetDays() / 7.f) * 7, 0, 0, 0); // Subtract the weeks
														// Assume 7d/w
		followDurationString.Append(FString::FromInt(followDuration.GetDays()) + TEXT("d"));
		followDuration -= FTimespan(followDuration.GetDays(), 0, 0, 0); // Subtract the days

		followDurationString.Append(FString::FromInt(followDuration.GetHours()) + TEXT("h"));
		followDuration -= FTimespan(followDuration.GetHours(), 0, 0); // Subtract the hours

		followDurationString.Append(FString::FromInt(followDuration.GetMinutes()) + TEXT("m"));
		followDuration -= FTimespan(0, followDuration.GetMinutes(), 0); // Subtract the minutes TODO this isn't needed?
	}

	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the twitch followers <duration> command to enable followers only chat.
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.followers %s\n"), *(Chat->Channel), *followDurationString)); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::DisableollowersOnlyChat(UTwitchChatConnector * Chat)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }


	// Attempt to get the async task (the one that actually operates the IRC socket)
	FTwitchChatConnectorTask *asyncTask = Chat->AsyncWorkTask;
	if (!asyncTask)
	{
		// Something went wrong getting the async task!
		UE_LOG(LogTwitchWorks, Error, TEXT("Failed to get async task!"));
	}
	else
	{
		// Use the twitch followersoff command to disable followers only chat.
		return asyncTask->SendOverSocket(FString::Printf(TEXT("PRIVMSG #%s :.followersoff\n"), *(Chat->Channel))); // TODO should the others switch to .command too?
	}

	// If we hit this that means something went wrong!
	return false;
}

bool UTwitchPluginBPLibrary::CreateTwitchCommand(UTwitchChatConnector *Chat, FString CommandName, FText CommandDescription, FText Usage, int NumParameters, bool bShowsUpInHelp, FTwitchCommandDelegate Binding)
{
	// Make sure everything is valid/safe
	FTwitchCommand Command;
	Command.CommandName = CommandName;
	Command.CommandDescription = CommandDescription;
	Command.Usage = Usage;
	Command.Parameters.SetNum(NumParameters);
	Command.showsUpInHelp = bShowsUpInHelp;
	FTwitchCommandBinding CommandBinding{ Command, Binding };
	if (!Chat || !CommandBinding.Binding.IsBound()) { return false; }

	// Attempt to find a command with the same name already registered.
	for (FTwitchCommandBinding Other : Chat->Commands)
	{
		// Compare the commands by name.
		if (Other.Command.CommandName == CommandBinding.Command.CommandName)
		{
			// Already a command with the same name registered! This isn't allowed!
			UE_LOG(LogTwitchWorks, Warning, TEXT("Command %s has already been registered!"), *CommandBinding.Command.CommandName);
			return false;
		}
	}


	for (FTwitchCommand Other : GetMutableDefault<UTwitchRuntimeSettings>()->Commands)
	{
		// Compare the commands by name.
		if (Other.CommandName == CommandBinding.Command.CommandName)
		{
			// Command hasn't been registered but is a "reserved" command defined in the settings! This isn't allowed!
			UE_LOG(LogTwitchWorks, Warning, TEXT("Attempted to use CreateTwitchCommand on a command (%s) that has already been set up in the settings menu! Please use BindToTwitchCommand instead!"), *CommandBinding.Command.CommandName);
			return false;
		}
	}

	// No errors found? Add it to the commands array!
	Chat->Commands.Add(CommandBinding);
	return true;
}

bool UTwitchPluginBPLibrary::UpdateTwitchCommand(UTwitchChatConnector *Chat, FString CommandName, FTwitchCommandBinding NewCommandBinding)
{
	// Make sure everything is valid/safe
	if (!Chat || !NewCommandBinding.Binding.IsBound()) { return false; }

	// Go over the chat's commands and try to find a command with a matching name.
	for (int i = 0, num= Chat->Commands.Num(); i < num; i++)
	{
		// Compare the commands by name
		FTwitchCommandBinding &Command = Chat->Commands[i];
		if (Command.Command.CommandName == NewCommandBinding.Command.CommandName)
		{
			// Found the command! Remove it and replace it with the new command binding data!
			Chat->Commands.RemoveAt(i);
			Chat->Commands.Add(NewCommandBinding);
			return true;
		}
	}


	// Wasn't able to find command, so also unable to update it.
	return false;
}

bool UTwitchPluginBPLibrary::RemoveTwitchCommand(UTwitchChatConnector *Chat, FString CommandName)
{
	if (!Chat) { return false; }

	// Go over the chat's commands and try to find a command with the correct name.
	for (int i = 0, num = Chat->Commands.Num(); i < num; i++)
	{
		// Compare the binding's name to the required name
		if (Chat->Commands[i].Command.CommandName == CommandName)
		{
			// Found the command! Remove it and/or unbind from it.
			Chat->Commands.RemoveAt(i);
			return true;
		}
	}

	// The command wasn't found so couldn't be removed.
	return false;
}

bool UTwitchPluginBPLibrary::GetRegisteredTwitchCommands(UTwitchChatConnector * Chat, TArray<FTwitchCommandBinding> &Commands)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Make a copy of the commands array so blueprint users don't have to worry about mutating the array.
	Commands = TArray<FTwitchCommandBinding>(Chat->Commands); 
	return true;
}

bool UTwitchPluginBPLibrary::BindToTwitchCommand(UTwitchChatConnector *Chat, FString CommandName, FTwitchCommandDelegate Delegate)
{
	// Is the passed in chat valid?
	if (!Chat) { return false; }

	// Try to find the command in the commands set up in the settings
	for (FTwitchCommand Command : GetMutableDefault<UTwitchRuntimeSettings>()->Commands)
	{
		// Compare the commands by name
		if (Command.CommandName == CommandName)
		{
			// Found the correct command.

			// Create a command binding for this command/delegate combo.
			FTwitchCommandBinding commandBinding;
			commandBinding.Command = Command;
			commandBinding.Binding = Delegate;

			// Make sure everything is valid/safe
			if (!Chat || !commandBinding.Binding.IsBound()) { return false; }

			// Attempt to find a command with the same name already registered (to avoid duplicates).
			for (FTwitchCommandBinding CommandBindingIt : Chat->Commands)
			{
				// Compare the commands by name.
				if (CommandBindingIt.Command.CommandName == commandBinding.Command.CommandName)
				{
					// Already a command with the same name registered! This isn't allowed!
					UE_LOG(LogTwitchWorks, Warning, TEXT("Command %s has already been registered!"), *commandBinding.Command.CommandName);
					return false;
				}
			}

			// No errors found? Add it to the commands array!
			Chat->Commands.Add(commandBinding);
			return true;
		}
	}

	// If we hit this that means we didn't find the command, it's probably not registered in the settings.
	UE_LOG(LogTwitchWorks, Warning, TEXT("Unable to bind to Twitch command: %s! Did you misspell the command's name or forget to set it up? (ERR_COMMAND_NOT_FOUND)"), *CommandName)
	return false;
}

void UTwitchPluginBPLibrary::DisconnectFromChat(UTwitchChatConnector *Chat)
{
	if (!Chat) { return; }

	// Is the async task and chat still running?
	if (Chat->bIsChatActive && Chat->AsyncWorkTask)
	{
		// Leave the IRC channel and stop the task
		Chat->AsyncWorkTask->SendOverSocket(FString::Printf(TEXT("PART #%s"), *(Chat->Channel)));
		Chat->AsyncWorkTask->StopTask(); // Stop the work in the task so it can gracefully abandon + stop listening
		// Ensure the destroy/other functionality knows the chat is no longer active.
		Chat->bIsChatActive = false;
	}


	// Seeing as we've disconnected from the chat it's worth nothing, so lets delete it (by setting it to a nullptr and letting GC do its thing
	Chat = nullptr;
}

void UTwitchPluginBPLibrary::BindToOnNewSubscription(UTwitchChatConnector *Chat, FTwitchNewSubscriptionDelegate Event)
{
	if (!Chat) { return; }

	Chat->OnNewSubscription = Event;
}

void UTwitchPluginBPLibrary::BindToOnRitual(UTwitchChatConnector *Chat, FTwitchRitualDelegate Event)
{
	if (!Chat) { return; }

	Chat->OnRitual = Event;
}

void UTwitchPluginBPLibrary::BindToOnPointsEvent(UTwitchChatConnector *Chat, FTwitchPointsEventDelegate Event)
{
	if (!Chat) { return; }

	Chat->OnPointsEvent = Event;
}

void UTwitchPluginBPLibrary::BindToOnRaid(UTwitchChatConnector* Chat, FTwitchRaidEventDelegate Event)
{
	if (!Chat) { return; }

	Chat->OnRaidEvent = Event;
}

FString UTwitchPluginBPLibrary::GetBadgeIdentifier(FTwitchBadge Badge)
{
	return Badge.RawBadgeCode;
}

void UTwitchPluginBPLibrary::BindToOnWhisper(UTwitchChatConnector* Chat, FTwitchWhisperDelegate Event)
{
	if (!Chat) { return; }

	Chat->OnWhisperEvent = Event;
}

FTwitchChatMode UTwitchPluginBPLibrary::GetChatModes(UTwitchChatConnector* Chat)
{
	if (!Chat) { return FTwitchChatMode(); }
	return Chat->ChatMode;
}
