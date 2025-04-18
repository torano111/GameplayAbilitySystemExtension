// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFeatureAction.h"
#include "UObject/SoftObjectPath.h"
#include "GameFeatureAction_AddGameplayCuePath.generated.h"

/**
 * GameFeatureAction responsible for adding gameplay cue paths to the gameplay cue manager.
 * This action MUST be added to data asset of GameFeatureData, NOT of UGASXExperienceDefinition or UGASXExperienceActionSet.
 *
 * @see UGASXGameFeature_AddGameplayCuePaths, UAbilitySystemGlobals::GameplayCueNotifyPaths
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Gameplay Cue Path"))
class UGameFeatureAction_AddGameplayCuePath final : public UGameFeatureAction
{
	GENERATED_BODY()

public:

	UGameFeatureAction_AddGameplayCuePath();

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	// This class just provides the path. For actual adding-path logic, see UGASXGameFeature_AddGameplayCuePaths
	const TArray<FDirectoryPath>& GetDirectoryPathsToAdd() const { return DirectoryPathsToAdd; }

private:
	/** List of paths to register to the gameplay cue manager. These are relative tot he game content directory */
	UPROPERTY(EditAnywhere, Category = "Game Feature | Gameplay Cues", meta = (RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> DirectoryPathsToAdd;
};
