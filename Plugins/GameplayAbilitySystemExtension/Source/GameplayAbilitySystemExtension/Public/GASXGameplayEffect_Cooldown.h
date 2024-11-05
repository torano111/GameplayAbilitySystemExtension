// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GASXGameplayEffect_Cooldown.generated.h"

/**
 * GameplayEffect for custom cooldown.
 * This is just Duration GE with UGASXMMC_Cooldown as the duration magnitude.
 * This class doesn't have cooldown tags, and it is expected to be used with UGASXGameplayAbility. UGASXGameplayAbility adds the tags dynamically. 
 * Also, UGASXMMC_Cooldown relies on UGASXGameplayAbility.
 */
UCLASS(NotBlueprintable)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXGameplayEffect_Cooldown : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGASXGameplayEffect_Cooldown();
	
};
