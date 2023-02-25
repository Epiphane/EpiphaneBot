// Copyright 2017-2022 HowToCompute. All Rights Reserved.


#include "PointsReward.h"

TSharedPtr<FJsonObject> FTwitchPointsReward::ToJSON()
{
	TSharedPtr<FJsonObject> JSONObject = MakeShareable(new FJsonObject);

	// Set the required params
	JSONObject->SetStringField(TEXT("title"), Title);
	JSONObject->SetNumberField(TEXT("cost"), Cost);

	// Set whether or not it is enabled
	JSONObject->SetBoolField(TEXT("is_enabled"), bIsEnabled);
	
	// TwiWorks currently only supports rewards with text input, so ensure that that property is enabled to avoid potential confusion/thinking why rewards aren't showing up without text input
	JSONObject->SetBoolField(TEXT("is_user_input_required"), true);
	if (!PromptText.IsEmpty())
	{
		// TODO: Once we enable support for points events without text input, selectively enable is_user_input_required rather than forcing it to true
		JSONObject->SetStringField(TEXT("prompt"), PromptText);
	}


	// Essentially ToHex(), but without the alpha channel. Does not appear to be a built-in solution for this and Twitch will reject RGBA hex strings
	FString RGBHex = FString::Printf(TEXT("%02X%02X%02X"), BackgroundColor.R, BackgroundColor.G, BackgroundColor.B);
	JSONObject->SetStringField(TEXT("background_color"), TEXT("#") + RGBHex); // NOTE: Twitch requires the hashtag infront of the hex value and UE4 only provides the numbers/letters by default

	// Set the (other) optional params only if they were provided
	if (MaxPerSteam > 0)
	{
		JSONObject->SetBoolField(TEXT("is_max_per_stream_enabled"), true);
		JSONObject->SetNumberField(TEXT("max_per_stream"), MaxPerSteam);
	}
	
	if (MaxPerUserPerSteam > 0)
	{
		JSONObject->SetBoolField(TEXT("is_max_per_user_per_stream_enabled"), true);
		JSONObject->SetNumberField(TEXT("max_per_user_per_stream"), MaxPerUserPerSteam);
	}
	
	if (GlobalCooldown > 0)
	{
		JSONObject->SetBoolField(TEXT("is_global_cooldown_enabled"), true);
		JSONObject->SetNumberField(TEXT("global_cooldown_seconds"), GlobalCooldown);
	}

	JSONObject->SetBoolField(TEXT("should_redemptions_skip_request_queue"), bInstantFulfillment);

	
	// Log the serialized thing
	FString JSONDump;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JSONDump);
	FJsonSerializer::Serialize(JSONObject.ToSharedRef(), JsonWriter);
	UE_LOG(LogTemp, Log, TEXT("Serialized PointsReward: %s"), *JSONDump);

	
	// Finished serializing!
	return JSONObject;

}

FTwitchPointsReward FTwitchPointsReward::FromJSON(TSharedPtr<FJsonObject> JSONObject)
{
	FTwitchPointsReward Reward;
	
	Reward.BackgroundColor = FColor::FromHex(JSONObject->GetStringField(TEXT("background_color")));
	Reward.Cost = JSONObject->GetNumberField(TEXT("cost"));
	Reward.PromptText = JSONObject->GetStringField(TEXT("prompt"));
	Reward.Title = JSONObject->GetStringField(TEXT("title"));
	Reward.bInstantFulfillment = JSONObject->GetBoolField(TEXT("should_redemptions_skip_request_queue"));
	Reward.bIsEnabled = JSONObject->GetBoolField(TEXT("is_enabled"));

	TSharedPtr<FJsonObject> MaxPerStreamSetting = JSONObject->GetObjectField(TEXT("max_per_stream_setting"));
	if (MaxPerStreamSetting.IsValid() && MaxPerStreamSetting->GetBoolField(TEXT("is_enabled")))
	{
		Reward.MaxPerSteam = MaxPerStreamSetting->GetNumberField(TEXT("max_per_stream"));
	}
	else
	{
		// Disabled or invalid
		Reward.MaxPerSteam = -1;
	}
	TSharedPtr<FJsonObject> GlobalCooldownSetting = JSONObject->GetObjectField(TEXT("global_cooldown_setting"));
	if (GlobalCooldownSetting.IsValid() && GlobalCooldownSetting->GetBoolField(TEXT("is_enabled")))
	{
		Reward.GlobalCooldown = GlobalCooldownSetting->GetNumberField(TEXT("global_cooldown_seconds"));
	}
	else
	{
		// Disabled or invalid
		Reward.GlobalCooldown = -1;
	}
	
	TSharedPtr<FJsonObject> MaxPerUserPerStreamSetting = JSONObject->GetObjectField(TEXT("max_per_user_per_stream_setting"));
	if (MaxPerUserPerStreamSetting.IsValid() && MaxPerUserPerStreamSetting->GetBoolField(TEXT("is_enabled")))
	{
		Reward.MaxPerUserPerSteam = MaxPerUserPerStreamSetting->GetNumberField(TEXT("max_per_user_per_stream"));
	}
	else
	{
		// Disabled or invalid
		Reward.MaxPerUserPerSteam = -1;
	}

	return Reward;
}

FTwitchPointsRedemption FTwitchPointsRedemption::FromJSON(TSharedPtr<FJsonObject> JSONObject)
{
	FTwitchPointsRedemption Redemption;
	// Basic info
	Redemption.RedemptionID = JSONObject->GetStringField(TEXT("id"));

	// User/redeemer info
	Redemption.RedeemerID = JSONObject->GetStringField(TEXT("user_id"));
	Redemption.RedeemerUsername = JSONObject->GetStringField(TEXT("user_name"));

	FString RedemptionStatus = JSONObject->GetStringField(TEXT("status"));
	if (RedemptionStatus == TEXT("UNFULFILLED"))
	{
		Redemption.Status = ETwitchRedemptionStatus::RS_UNFULFILLED;
	}
	else if (RedemptionStatus == TEXT("FULFILLED"))
	{
		Redemption.Status = ETwitchRedemptionStatus::RS_FULFILLED;
	}
	else
	{
		// NOTE: Assume cancelled for cases where it's unknown as well
		Redemption.Status = ETwitchRedemptionStatus::RS_CANCELED;
	}

	// Finished parsing!
	return Redemption;
}
