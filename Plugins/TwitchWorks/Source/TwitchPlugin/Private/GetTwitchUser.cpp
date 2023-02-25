// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "GetTwitchUser.h"
#include "TwitchPlugin.h"
#include "Engine/Texture2D.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "ImageHelpers.h"
#include "Json.h"

UGetTwitchUser::UGetTwitchUser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGetTwitchUser *UGetTwitchUser::GetTwitchUser(UTwitchChatConnector* _Chat, FString _UserName)
{
	// Create a mutable instance of this class, allow it to set itself up and return it to blueprint so the callbacks will work
	UGetTwitchUser *Task = NewObject<UGetTwitchUser>();
	Task->Chat = _Chat;
	Task->Username = _UserName;
	return Task;
}

void UGetTwitchUser::Activate()
{
	// Attempt to extract the OAuth token from the chat (or last login if applicable during the transition phase)
	if (!Chat || Chat->ConnectedUserCreds.OAuthToken.IsEmpty())
	{
		if (FTwitchPluginModule::LastOAuthToken.IsEmpty())
		{
			OnFail.Broadcast(FTwitchUserAccount());
			UE_LOG(LogTwitchWorks, Warning, TEXT("Invalid Chat Credentials passed to GetTwitchEmoteSet! Please note that credentials (passed through the chat) are now required for all Helix API calls (the old Kraken API has unfortunately been depricated and will soon be removed)!"));
			return;
		}
		else
		{
			// No chat passed in, but we do have a previous successful login. Attempt to use this OAuth token for the request!
			// TODO: Remove, this is temporary to preserve backwards compatibility until users can change their code
			UE_LOG(LogTwitchWorks, Warning, TEXT("Deprication Notice: GetTwitchEmoteSet now required OAuth credentials to call the Twitch API, meaning you must pass in the chat. This appears to not currently be the case, but we detected that you did previously log in successfully. Using these credentials for now, but please note that this will be removged in the future!"))
			ExtractedOAuthToken = FTwitchPluginModule::LastOAuthToken;
		}
	}
	else
	{
		ExtractedOAuthToken = Chat->ConnectedUserCreds.OAuthToken;
	}

	CreateHTTPRequest(Username);
}

void UGetTwitchUser::CreateHTTPRequest(FString UserName)
{
	// Create an HTTP request for the user lookup
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ExtractedOAuthToken));

	// TODO if this becomes mutli-user request in the future, the list of users is comma-seperated
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/users?login=%s"), *UserName)); 

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetTwitchUser::HandleHTTPRequest);
	HttpRequest->ProcessRequest();

}

void UGetTwitchUser::CreateFollowsHTTPRequest(FString UserID, int32 Offset)
{
	// Create an HTTP request for the following lookup
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ExtractedOAuthToken));

	// Request the channels the user is following - passing along the limit of 100.
	HttpRequest->SetURL(FString::Printf(TEXT("https://api.twitch.tv/helix/users/follows?from_id=%s&limit=100&offset=%i"), *UserID, Offset));

	// Bind the HandleHTTPRequest function to the request complete delegate and execute the HTTP request
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGetTwitchUser::HandleFollowingHTTPRequest);
	HttpRequest->ProcessRequest();
}

void UGetTwitchUser::CreateLogoHTTPRequest(FString LogoURL)
{
	FImageDownloader* ImageRequest = new FImageDownloader();
	ImageRequest->OnSuccess.BindUObject(this, &UGetTwitchUser::OnSuccessfulLogoFetch);
	ImageRequest->OnFail.BindUObject(this, &UGetTwitchUser::OnFailedLogoFetch);
	ImageRequest->GetImage(LogoURL);
}

void UGetTwitchUser::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0 || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		// Something went wrong getting the HTTP request - log out an error!
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to get twitch user! (ERR_INVALID_INITIAL_RESPONSE)"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\n%s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());

		// Create a blank image to avoid possible nullptr issues and call the OnFail delegate
		TwitchUser.Logo = NewObject<UTexture2D>();
		OnFail.Broadcast(TwitchUser);
		return;
	}

	// Attempt to turn the load the response's json
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		// Deal with the fact twitch supports getting multiple users at once (we're only getting one)
		TArray<TSharedPtr<FJsonValue>> UsersJSONValues = JSONObject->GetArrayField(TEXT("data"));

		// Opt for the last user for easy portability to fetching multiple users at once (but only 1 itteration should happen)
		for (TSharedPtr<FJsonValue> UserJSONValue : UsersJSONValues)
		{
			// Create a TwitchUserAccount struct based on the json
			TSharedPtr<FJsonObject> UserObject = UserJSONValue->AsObject();
			FTwitchUserAccount User;
			// The user' id
			User.Id = UserObject->GetStringField(TEXT("id"));
			
			// The user's profile creation date
			FDateTime::ParseHttpDate(*UserObject->GetStringField(TEXT("created_at")), User.CreationDate);
			
			// The user's display name / nickname
			User.DisplayName = UserObject->GetStringField(TEXT("display_name"));
			
			// The user's username
			User.UserName = UserObject->GetStringField(TEXT("login"));
			
			// The user's user type
			User.UserType = UserObject->GetStringField(TEXT("type"));

			// Save the user as a local class variable for the logo callback to use
			TwitchUser = User;

			// Attempt to get the user's logo and resort to the default logo if the user doesn't have a custom profile image
			FString LogoUrl = UserObject->HasTypedField<EJson::String>("profile_image_url") ? UserObject->GetStringField(TEXT("profile_image_url")) : TEXT("https://static-cdn.jtvnw.net/jtv_user_pictures/xarth/404_user_300x300.png");
			if (LogoUrl.Len() > 0) // (Probably) valid url?
			{
				CreateLogoHTTPRequest(LogoUrl); // Logo will also call OnSuccess TODO make OnFair for all of these async tasks
				return;
			}
			else
			{
				// Something went wrong! The logo somehow ended up being in the json but being 0 of length?
				UE_LOG(LogTwitchWorks, Error, TEXT("User profile_image_url parameter invalid! (length = 0)"));
				// Create a default image to avoid nullptr issues
				User.Logo = NewObject<UTexture2D>();
				OnFail.Broadcast(User);
				return;
			}
		}
	}
}

void UGetTwitchUser::OnSuccessfulLogoFetch(UTexture2D* Logo)
{
	// Set the logo filed in the user' details and call the delegate
	TwitchUser.Logo = Logo;
	return CreateFollowsHTTPRequest(TwitchUser.Id);
}

void UGetTwitchUser::OnFailedLogoFetch(UTexture2D* Logo)
{
	TwitchUser.Logo = NewObject<UTexture2D>(); // Create a default image to avoid nullptr issues
	OnFail.Broadcast(TwitchUser);
	return;
}


void UGetTwitchUser::HandleFollowingHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (!bSucceeded || HttpResponse->GetContentLength() <= 0)
	{
		// Something went wrong with the HTTP request - log out an error / show the HTTP response
		UE_LOG(LogTwitchWorks, Warning, TEXT("Failed to get twitch user! (ERR_GET_USER_FOLLOWERS)"));
		UE_LOG(LogTwitchWorks, Log, TEXT("HTTP Response Code: %i\n%s"), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString());
		OnFail.Broadcast(TwitchUser);
		return;
	}

	// Attempt to turn the load the response's json
	TSharedPtr<FJsonObject> JSONObject;
	TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JSONReader, JSONObject))
	{
		// Get the returned following values so we can iterate over them.
		TArray<TSharedPtr<FJsonValue>> FollowsJSONValues = JSONObject->GetArrayField(TEXT("data"));

		
		for (TSharedPtr<FJsonValue> FollowJSONValue : FollowsJSONValues)
		{
			// Create a TwitchChannelFollow struct based on the json
			TSharedPtr<FJsonObject> FollowObject = FollowJSONValue->AsObject();

			// Double-check that this follows object is valid!
			if (!FollowObject.IsValid()) { continue; }
			
			// "Assert" thta the channel is actually in the follow object; skip it if it isn't in there.
			//if (!FollowObject->HasTypedField<EJson::Object>(TEXT("channel"))) { UE_LOG(LogTwitchWorks, Warning, TEXT("Error getting followers! Follow object does not contain channel object. Skipping...")); continue; }
			//TSharedPtr<FJsonObject> ChannelObject = FollowObject->GetObjectField(TEXT("channel"));
			FTwitchChannelFollow Follow;

			// The channel's id, means to be _id & a string (atleast, the documentation tells us to do this)
			Follow.Id = FollowObject->GetStringField(TEXT("to_id"));

			// The channel's name
			Follow.Name = FollowObject->GetStringField(TEXT("to_login"));

			// The channel's display name
			Follow.DisplayName = FollowObject->GetStringField(TEXT("to_name"));

			// The date/time at which the user followed the channel.
			FDateTime::ParseHttpDate(*FollowObject->GetStringField(TEXT("followed_at")), Follow.FollowDate);
			UE_LOG(LogTemp, Log, TEXT("Deprication Warning: Due to Twitch API requirements, and impending deprication of the V5 API the 'NumFollowers', 'CreationDate', 'ChannelActiveGame' and 'ChannelStatus' properties have been removed from the user following array."))
			/*
			// Get the channel's number of followers
			Follow.NumFollowers = ChannelObject->GetNumberField(TEXT("followers"));

			FDateTime::ParseIso8601(*ChannelObject->GetStringField(TEXT("created_at")), Follow.CreationDate);

			// The game that is currently active on the channel (NO_ACTIVE_GAME if no game is given).
			Follow.ChannelActiveGame = ChannelObject->HasTypedField<EJson::Null>(TEXT("game")) || ChannelObject->HasTypedField<EJson::None>(TEXT("game")) ? TEXT("NO_ACTIVE_GAME") : ChannelObject->GetStringField(TEXT("game"));

			// The status message of the channel.
			Follow.ChannelStatus = ChannelObject->GetStringField(TEXT("status"));
			*/

			// Now that we've successfully created the follow object, we can add it to the array of (channels the user) follows.
			TwitchUser.Following.Add(Follow);
		}


		// Did we get all followers? (or are there more to be fetched?)
		if (JSONObject->HasTypedField<EJson::Number>(TEXT("_total")) && JSONObject->GetIntegerField(TEXT("_total")) > TwitchUser.Following.Num())
		{
			// We haven't received all of the channels the user follows yet! Request it again using an offset (ie the number of follows we already have) .
			return CreateFollowsHTTPRequest(TwitchUser.Id, TwitchUser.Following.Num());
		}

		// If we got here that means we have all of the required data! Return the result!
		OnSuccess.Broadcast(TwitchUser);
		return;
	}
	else
	{
		// If we got here, then something must have gone wrong. Looks like we couldn't get the channels the user is following. Return what we have and log an error.
		UE_LOG(LogTwitchWorks, Error, TEXT("Unable to receive the channels the user is following! Fatal error getting user details!"))
		OnFail.Broadcast(TwitchUser);
		return;
	}

	
}