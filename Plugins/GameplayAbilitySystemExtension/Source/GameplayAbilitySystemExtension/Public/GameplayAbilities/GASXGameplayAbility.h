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

	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
	TArray<TObjectPtr<class UGASXAbilityCost>> AdditionalCosts;

	// Custom cooldown duration for UGASXGameplayEffect_Cooldown
	UPROPERTY(EditDefaultsOnly, Category = Cooldowns, meta = (EditCondition = "IsUsingGASXCooldownGEClass()"))
	FScalableFloat CooldownDuration;

	// Custom cooldown tags for UGASXGameplayEffect_Cooldown
	UPROPERTY(EditDefaultsOnly, Category = Cooldowns, meta = (EditCondition = "IsUsingGASXCooldownGEClass()"))
	FGameplayTagContainer CooldownTags;

protected:
	// Temporary tag container that we will return the pointer to in GetCooldownTags().
	// This will be a union of CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer UnitedCooldownTags;


public:
	UGASXGameplayAbility();

	// UGameplayAbility interface
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override; /** Called when the avatar actor is set/changes */
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override; /** Returns all tags that can put this ability into cooldown */
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override; /** Applies CooldownGameplayEffect to the target */
	// End of UGameplayAbility interface

	// Checks if CooldownGameplayEffectClass is a child class of UGASXGameplayEffect_Cooldown
	UFUNCTION(BlueprintPure, Category = Ability)
	bool IsUsingGASXCooldownGEClass() const;

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