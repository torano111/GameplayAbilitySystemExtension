// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GASXDataTypes.generated.h"

/**
 * Struct defining a list of gameplay effects, a tag, and targeting info
 * These containers are defined statically in blueprints or assets and then turn into Specs at runtime
 * This code is from ActionRPG sample.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYABILITYSYSTEMEXTENSION_API FGASXGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FGASXGameplayEffectContainer() {}

	/** Sets the way that targeting happens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TSubclassOf<class UGASXTargetType> TargetType;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<TSubclassOf<class UGameplayEffect>> TargetGameplayEffectClasses;
};

/** 
 * A "processed" version of GASXGameplayEffectContainer that can be passed around and eventually applied 
 * This code is from ActionRPG sample.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYABILITYSYSTEMEXTENSION_API FGASXGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:
	FGASXGameplayEffectContainerSpec() {}

	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	FGameplayAbilityTargetDataHandle TargetData;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
	TArray<struct FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/** Returns true if this has any valid effect specs */
	bool HasValidEffects() const;

	/** Returns true if this has any valid targets */
	bool HasValidTargets() const;

	/** Adds new targets to target data */
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	void ClearTargetData();
};
