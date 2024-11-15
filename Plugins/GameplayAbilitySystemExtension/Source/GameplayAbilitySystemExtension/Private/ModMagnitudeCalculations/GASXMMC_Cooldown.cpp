// Copyright 2024 Toranosuke Ichikawa

#include "ModMagnitudeCalculations/GASXMMC_Cooldown.h"
#include "GameplayAbilities/GASXGameplayAbility.h"

float UGASXMMC_Cooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	auto Ability = Cast<UGASXGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());
	return Ability == nullptr ? 0.f : Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
}
