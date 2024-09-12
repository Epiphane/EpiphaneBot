// Copyright Thomas Steinke.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EpiGameSettings.generated.h"

class UChatAvatar;

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName="EpiphaneBot Settings"))
class EPIPHANEBOT_API UEpiGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UChatAvatar> DefaultAvatar;
};
