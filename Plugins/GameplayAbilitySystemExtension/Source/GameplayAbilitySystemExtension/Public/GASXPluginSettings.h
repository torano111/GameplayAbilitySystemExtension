// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GASXPluginSettings.generated.h"

/**
 * 
 */
UCLASS(config = Engine, defaultconfig)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXPluginSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// This will be used if no other experience can be loaded.
	UPROPERTY(config, EditAnywhere, Category = "Experience", meta = (AllowedClasses = "/Script/GameplayAbilitySystemExtension.GASXExperienceDefinition"))
	FSoftObjectPath DefaultExperience;
	
public:
	UGASXPluginSettings();

	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return "Plugins"; }

#if WITH_EDITOR
	/** Gets the section text, uses the classes DisplayName by default. */
	virtual FText GetSectionText() const override;
	/** Gets the description for the section, uses the classes ToolTip by default. */
	virtual FText GetSectionDescription() const override;
#endif
};
