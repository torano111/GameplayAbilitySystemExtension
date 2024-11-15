// Copyright 2024 Toranosuke Ichikawa

#include "GameplayEffects/GASXGameplayEffect_Cooldown.h"
#include "ModMagnitudeCalculations/GASXMMC_Cooldown.h"

UGASXGameplayEffect_Cooldown::UGASXGameplayEffect_Cooldown()
	: Super()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	// Sets UGASXMMC_Cooldown as ModMagnitudeCalc class, which returns UGASXGameplayAbility's CooldownDuration as base magnitude.
	FCustomCalculationBasedFloat CustomCalculation;
	CustomCalculation.CalculationClassMagnitude = UGASXMMC_Cooldown::StaticClass();
	DurationMagnitude = FGameplayEffectModifierMagnitude(CustomCalculation);

	// No need to set cooldown tags because UGASXGameplayAbility adds the tags dynamically.
}
