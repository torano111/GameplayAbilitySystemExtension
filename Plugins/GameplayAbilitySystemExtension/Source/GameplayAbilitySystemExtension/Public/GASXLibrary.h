// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GASXLibrary.generated.h"

struct FGASXGameplayEffectContainerSpec;

/**
 * 
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Checks if spec has any effects */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveEffects(const FGASXGameplayEffectContainerSpec& ContainerSpec);

	/** Checks if spec has any targets */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveTargets(const FGASXGameplayEffectContainerSpec& ContainerSpec);

	/** Adds targets to a copy of the passed in effect container spec and returns it */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FGASXGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Applies container spec that was made from an ability */
	UFUNCTION(BlueprintCallable, Category = Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec);
};
