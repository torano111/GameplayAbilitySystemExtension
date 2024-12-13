// Copyright 2024 Toranosuke Ichikawa

#include "GameplayAbilities/GASXGameplayAbility.h"
#include "GASXAbilitySystemComponent.h"
#include "AbilityCosts/GASXAbilityCost.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GASXTargetType.h"
#include "GASXBaseCharacter.h"
#include "GameplayEffects/GASXGameplayEffect_Cooldown.h"
#include "AbilitySystemGlobals.h"

UGASXGameplayAbility::UGASXGameplayAbility()
	: Super()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationPolicy = EGASXAbilityActivationPolicy::OnInputTriggered;
}

void UGASXGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

bool UGASXGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// Custom cost check. Mostly from Lyra.

	// Verify we can afford any additional costs
	for (TObjectPtr<UGASXAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	return true;
}

void UGASXGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	check(ActorInfo);

	// Custom cost application. Mostly from Lyra.

	// Used to determine if the ability actually hit a target (as some costs are only spent on successful attempts)
	auto DetermineIfAbilityHitTarget = [&]()
		{
			if (ActorInfo->IsNetAuthority())
			{
				if (UGASXAbilitySystemComponent* ASC = Cast<UGASXAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
				{
					FGameplayAbilityTargetDataHandle TargetData;
					ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);
					for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
					{
						if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
						{
							return true;
						}
					}
				}
			}

			return false;
		};

	// Pay any additional costs
	bool bAbilityHitTarget = false;
	bool bHasDeterminedIfAbilityHitTarget = false;
	for (TObjectPtr<UGASXAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
			{
				if (!bHasDeterminedIfAbilityHitTarget)
				{
					bAbilityHitTarget = DetermineIfAbilityHitTarget();
					bHasDeterminedIfAbilityHitTarget = true;
				}

				if (!bAbilityHitTarget)
				{
					continue;
				}
			}

			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}

const FGameplayTagContainer* UGASXGameplayAbility::GetCooldownTags() const
{
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&UnitedCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the UnitedCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)

	if (ParentTags) MutableTags->AppendTags(*ParentTags);
	if (IsUsingGASXCooldownGEClass()) MutableTags->AppendTags(CooldownTags);

	return MutableTags;
}

void UGASXGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		if (IsUsingGASXCooldownGEClass()) SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);	// Pass cooldown tags to GE.
		// we don't pass CooldownDuration to GE, because UGASXGameplayEffect_Cooldown uses UGASXGameplayEffect_Cooldown which returns this class's CooldownDuration as base magnitude.

		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UGASXGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bBlocked = false;
	bool bMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}

	const UGASXAbilitySystemComponent* ASC = Cast<UGASXAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	// Expand our ability tags to add additional required/blocked tags
	if (ASC)
	{
		ASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}

	// Check to see the required/blocked tags for this ability
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;

		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	if (bBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (bMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
}

bool UGASXGameplayAbility::IsUsingGASXCooldownGEClass() const
{
	return CooldownGameplayEffectClass && CooldownGameplayEffectClass->IsChildOf(UGASXGameplayEffect_Cooldown::StaticClass());
}

FGASXGameplayEffectContainerSpec UGASXGameplayAbility::MakeEffectContainerSpecFromContainer(const FGASXGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	// First figure out our actor info
	FGASXGameplayEffectContainerSpec ReturnSpec;

	// If we have a target type, run the targeting logic. This is optional, targets can be added later
	if (Container.TargetType.Get())
	{
		TArray<FHitResult> HitResults;
		TArray<AActor*> TargetActors;
		const UGASXTargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		TargetTypeCDO->GetTargets(GetActorInfo(), EventData, HitResults, TargetActors);
		ReturnSpec.AddTargets(HitResults, TargetActors);
	}

	// If we don't have an override level, use the default on the ability itself
	if (OverrideGameplayLevel == INDEX_NONE)
	{
		OverrideGameplayLevel = OverrideGameplayLevel = this->GetAbilityLevel(); //OwningASC->GetDefaultAbilityLevel();
	}

	// Build GameplayEffectSpecs for each applied effect
	for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
	{
		ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
	}
	return ReturnSpec;
}

FGASXGameplayEffectContainerSpec UGASXGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGASXGameplayEffectContainer* FoundContainer = EffectContainerMap.Find(ContainerTag);

	if (FoundContainer)
	{
		return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	}
	return FGASXGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> UGASXGameplayAbility::ApplyEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> UGASXGameplayAbility::ApplyEffectContainer(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FGASXGameplayEffectContainerSpec Spec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(Spec);
}

void UGASXGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EGASXAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			ASC->TryActivateAbility(Spec.Handle);
		}
	}
}
