// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GASXMMC_Cooldown.generated.h"

/**
 * Mod Magnitude Calculation for custom cooldown.
 * This just returns UGASXGameplayAbility's CooldownDuration as base magnitude.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXMMC_Cooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UGASXMMC_Cooldown() { };

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
