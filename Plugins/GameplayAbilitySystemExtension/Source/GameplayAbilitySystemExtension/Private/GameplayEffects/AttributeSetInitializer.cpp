// Copyright 2024 Toranosuke Ichikawa

#include "GameplayEffects/AttributeSetInitializer.h"
#include "AbilitySystemComponent.h"

bool FAttributeSetInitializer::IsValid() const
{
	for (const auto& AM : AttributeMagnitudes)
	{
		if (AM.Attribute.IsValid()) return true;
	}
	return false;
}

TArray<FGameplayModifierInfo> FAttributeSetInitializer::MakeGameplayModifierInfo() const
{
	TArray<FGameplayModifierInfo> Result;
	if (IsValid())
	{
		for (const auto& AM : AttributeMagnitudes)
		{
			if (AM.Attribute.IsValid())
			{
				FGameplayModifierInfo ModInfo;
				ModInfo.ModifierOp = EGameplayModOp::Override;
				ModInfo.Attribute = AM.Attribute;
				ModInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(AM.Magnitude));
				Result.Add(ModInfo);
			}
		}
	}
	return Result;
}

UGameplayEffect* FAttributeSetInitializer::MakeGameplayEffect() const
{
	if (IsValid())
	{
		auto ModInfos = MakeGameplayModifierInfo();
		if (ModInfos.Num() > 0)
		{
			UGameplayEffect* GE = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AttributeSetInitializerGameplayEffect")));
			GE->DurationPolicy = EGameplayEffectDurationType::Instant;
			for (auto ModInfo : ModInfos)
			{
				GE->Modifiers.Add(ModInfo);
			}
			return GE;
		}
	}
	return nullptr;
}

FActiveGameplayEffectHandle FAttributeSetInitializer::Apply(UAbilitySystemComponent* ASC, float Level, const FGameplayEffectContextHandle& EffectContext, FPredictionKey PredictionKey) const
{
	if (ASC != nullptr)
	{
		if (auto GE = MakeGameplayEffect())
		{
			ASC->ApplyGameplayEffectToSelf(GE, Level, EffectContext, PredictionKey);
		}
	}
	return FActiveGameplayEffectHandle();
}
