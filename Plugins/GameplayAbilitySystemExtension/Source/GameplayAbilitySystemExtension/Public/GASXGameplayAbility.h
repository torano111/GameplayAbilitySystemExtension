// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASXDataTypes.h"
#include "GASXGameplayAbility.generated.h"

/**
 * Base GameplayAbility
 */
UCLASS(Abstract)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	// Activates this ability immediately when granted. Used for passive abilities.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateAbilityOnGranted = false;

	/** Map of gameplay tags to gameplay effect containers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
	TMap<FGameplayTag, FGASXGameplayEffectContainer> EffectContainerMap;

protected:
	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<class UGASXAbilityCost>> AdditionalCosts;

public:
	UGASXGameplayAbility();

	// UGameplayAbility interface
	/** Called when the avatar actor is set/changes */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	// End of UGameplayAbility interface

	/** Make gameplay effect container spec to be applied later, using the passed in container. This also runs targeting logic if the efffect container has a target type. */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FGASXGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FGASXGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap. This also runs targeting logic if the matched effect container has a target type. */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual FGASXGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);

	/** Applies a gameplay effect container spec that was previously created. This does NOT run targeting logic unlike ApplyEffectContainer(). */
	UFUNCTION(BlueprintCallable, Category = Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec);

	/** Applies a gameplay effect container, by creating and then applying the spec. This also runs targeting logic if the matched effect container has a target type.*/
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel = -1);
};
