// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GASXUserFacingExperienceDefinition.generated.h"

class FString;
class UObject;
class UTexture2D;
class UUserWidget;
struct FFrame;

/**
 * Description of settings used to display experiences in the UI and start a new session
 */
UCLASS(BlueprintType)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXUserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** The specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** The gameplay experience to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "GASXExperienceDefinition"))
	FPrimaryAssetId ExperienceID;

	/** Extra arguments passed as URL options to the game */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	TMap<FString, FString> ExtraArgs;

	/** Primary title in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileTitle;

	/** Secondary title */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileSubTitle;

	/** Full description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileDescription;

	/** Icon used in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	TObjectPtr<UTexture2D> TileIcon;

	/** The loading screen widget to show when loading into (or back out of) a given experience */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = LoadingScreen)
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	static const FString ExperienceOptionName;

public:
	UFUNCTION(BlueprintCallable, Category = Experience)
	FString ConstructTravelURL() const;
};
