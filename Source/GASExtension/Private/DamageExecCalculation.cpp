// Copyright 2024 Toranosuke Ichikawa

#include "DamageExecCalculation.h"
#include "MyAttributeSet.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage)

	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, Damage, Source, true)
	}
};

static FDamageStatics DamageStatics()
{
	static FDamageStatics DamageStatics;
	return DamageStatics;
}

UDamageExecCalculation::UDamageExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void UDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FDamageStatics().DamageDef, EvaluationParameters, Damage);

	const float FinalDamage = FMath::Max(Damage, 0.f);
	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMyAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
}
