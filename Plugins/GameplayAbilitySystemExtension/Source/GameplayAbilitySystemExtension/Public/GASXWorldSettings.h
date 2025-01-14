// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GASXWorldSettings.generated.h"

class UGASXExperienceDefinition;

/**
 * The default world settings object, used primarily to set the default gameplay experience to use when playing on this map
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:

	AGASXWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

public:
	// Returns the default experience to use when a server opens this map if it is not overridden by the user-facing experience
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// The default experience to use when this map is opened if it is not overridden by the user-facing experience
	UPROPERTY(EditDefaultsOnly, Category = GameMode, meta = (AllowedClasses = "/Script/GameplayAbilitySystemExtension.GASXExperienceDefinition"))
	FSoftObjectPath DefaultGameplayExperience;
};
