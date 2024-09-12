// Copyright Thomas Steinke

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "Engine/DataAsset.h"
#include "ChatAvatar.generated.h"

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API UChatAvatar : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperSprite* BaseImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* SmallImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bColorable = true;
};
