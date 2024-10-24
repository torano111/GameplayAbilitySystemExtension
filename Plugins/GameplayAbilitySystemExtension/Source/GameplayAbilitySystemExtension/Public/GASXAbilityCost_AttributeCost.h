// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXAbilityCost.h"
#include "GASXAbilityCost_AttributeCost.generated.h"

/**
 * Ability Cost class to subtract cost from specified attribute, similar to CostGE
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXAbilityCost_AttributeCost : public UGASXAbilityCost
{
	GENERATED_BODY()

public:
	// Attribute to consume as cost.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayAttribute Attribute;

	// Custom Cost. This should return a negative value.
	UPROPERTY(EditAnywhere, Category = Costs)
	FGameplayEffectModifierMagnitude Cost;
	
public:
	virtual bool CheckCost(const UGASXGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UGASXGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	// make a new object of instant GE with addictive modifier at runtime.
	virtual UGameplayEffect* MakeCostGameplayEffect() const;
};
