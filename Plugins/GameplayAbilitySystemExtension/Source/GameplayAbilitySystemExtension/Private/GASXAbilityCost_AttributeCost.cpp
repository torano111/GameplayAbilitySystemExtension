// Copyright 2024 Toranosuke Ichikawa

#include "GASXAbilityCost_AttributeCost.h"
#include "AbilitySystemComponent.h"
#include "GASXGameplayAbility.h"
#include "AbilitySystemGlobals.h"

bool UGASXAbilityCost_AttributeCost::CheckCost(const UGASXGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	UGameplayEffect* CostGE = MakeCostGameplayEffect();

	// This code is from UGameplayAbility::CheckCost
	if (CostGE)
	{
		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent != nullptr);
		if (!AbilitySystemComponent->CanApplyAttributeModifiers(CostGE, Ability->GetAbilityLevel(Handle, ActorInfo), Ability->MakeEffectContext(Handle, ActorInfo)))
		{
			// Apply an activation failed tag.
			const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

			if (OptionalRelevantTags && CostTag.IsValid())
			{
				OptionalRelevantTags->AddTag(CostTag);
			}
			return false;
		}
	}
	return true;
}

void UGASXAbilityCost_AttributeCost::ApplyCost(const UGASXGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UGameplayEffect* CostGE = MakeCostGameplayEffect();

	if (CostGE)
	{
		UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent != nullptr);
		FGameplayEffectSpecHandle SpecHandle = Ability->MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostGE->GetClass(), Ability->GetAbilityLevel(Handle, ActorInfo));
		AbilitySystemComponent->ApplyGameplayEffectToSelf(CostGE, Ability->GetAbilityLevel(Handle, ActorInfo), AbilitySystemComponent->MakeEffectContext());
	}
}

UGameplayEffect* UGASXAbilityCost_AttributeCost::MakeCostGameplayEffect() const
{
	UGameplayEffect* CostGE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AttributeCostGE")));
	CostGE->DurationPolicy = EGameplayEffectDurationType::Instant;

	int32 Idx = CostGE->Modifiers.Num();
	CostGE->Modifiers.SetNum(Idx + 1);

	FGameplayModifierInfo& ModInfo = CostGE->Modifiers[Idx];
	ModInfo.ModifierMagnitude = Cost;
	ModInfo.ModifierOp = EGameplayModOp::Additive;
	ModInfo.Attribute = Attribute;

	return CostGE;
}
