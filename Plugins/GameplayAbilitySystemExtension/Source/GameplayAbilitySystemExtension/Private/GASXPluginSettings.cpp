// Copyright 2024 Toranosuke Ichikawa

#include "GASXPluginSettings.h"

#define LOCTEXT_NAMESPACE "GASXPluginSettings"

UGASXPluginSettings::UGASXPluginSettings()
	: Super()
{

}

#if WITH_EDITOR
FText UGASXPluginSettings::GetSectionText() const
{
	return LOCTEXT("UGASXDeveloperSettings::GetSectionText", "GameplayAbilitySystemExtension");
}

FText UGASXPluginSettings::GetSectionDescription() const
{
	return LOCTEXT("UGASXDeveloperSettings::GetSectionDescription", "GameplayAbilitySystemExtension Settings");
}
#endif

#undef LOCTEXT_NAMESPACE
