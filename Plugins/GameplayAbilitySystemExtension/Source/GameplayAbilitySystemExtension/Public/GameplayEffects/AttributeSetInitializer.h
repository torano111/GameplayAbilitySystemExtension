// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AttributeSetInitializer.generated.h"

class UAbilitySystemComponent;
struct FActiveGameplayEffectHandle;

USTRUCT(BlueprintType)
struct GAMEPLAYABILITYSYSTEMEXTENSION_API FAttributeMagnitude
{
	GENERATED_BODY()

public:
	/** The Attribute we modify or the GE we modify modifies. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FAttributeSetInitializer", meta = (FilterMetaTag = "HideFromModifiers"))
	FGameplayAttribute Attribute;

	// New magnitude of the attribute
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FAttributeSetInitializer")
	float Magnitude;
};

/**
 * Useful struct to initialize AttributeSet with a GameplayEffect.
 * EGameplayModOp::Override is used for FGameplayModifierInfo.
 * This supports only EGameplayEffectMagnitudeCalculation::ScalableFloat as MagnitudeCalculationType. If you want to use other type, use your own GameplayEffect.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYABILITYSYSTEMEXTENSION_API FAttributeSetInitializer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AttributeSetInitializer")
	TArray<FAttributeMagnitude> AttributeMagnitudes;

	bool IsValid() const;

	// Makes an array of FGameplayModifierInfo with override ModOp.
	TArray<FGameplayModifierInfo> MakeGameplayModifierInfo() const;

	// Makes an Instant GameplayEffect at runtime
	UGameplayEffect* MakeGameplayEffect() const;

	// Makes an GameplayEffect to initialize AttributeSet and applies it.
	FActiveGameplayEffectHandle Apply(UAbilitySystemComponent* ASC, float Level, const FGameplayEffectContextHandle& EffectContext, FPredictionKey PredictionKey = FPredictionKey()) const;
};
