// Copyright 2024 Toranosuke Ichikawa

#include "GASXGameplayAbility.h"

UGASXGameplayAbility::UGASXGameplayAbility()
	: Super()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}
