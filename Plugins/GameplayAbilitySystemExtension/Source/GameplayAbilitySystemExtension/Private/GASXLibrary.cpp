// Copyright 2024 Toranosuke Ichikawa

#include "GASXLibrary.h"
#include "GASXDataTypes.h"

bool UGASXLibrary::DoesEffectContainerSpecHaveEffects(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool UGASXLibrary::DoesEffectContainerSpecHaveTargets(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FGASXGameplayEffectContainerSpec UGASXLibrary::AddTargetsToEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FGASXGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> UGASXLibrary::ApplyExternalEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}
