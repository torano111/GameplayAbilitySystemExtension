// Copyright 2024 Toranosuke Ichikawa


#include "GASXDataTypes.h"

bool FGASXGameplayEffectContainerSpec::HasValidEffects() const
{
    return TargetGameplayEffectSpecs.Num() > 0;
}

bool FGASXGameplayEffectContainerSpec::HasValidTargets() const
{
    return TargetData.Num() > 0;
}

void FGASXGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}
}

void FGASXGameplayEffectContainerSpec::ClearTargetData()
{
	TargetData.Clear();
}
