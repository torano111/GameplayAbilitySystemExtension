// Copyright 2024 Toranosuke Ichikawa

#include "HealExecCalculation.h"
#include "MyAttributeSet.h"

struct FHealStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Healing)

	FHealStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMyAttributeSet, Healing, Source, true)
	}
};

static FHealStatics HealStatics()
{
	static FHealStatics HealStatics;
	return HealStatics;
}

UHealExecCalculation::UHealExecCalculation()
{
	RelevantAttributesToCapture.Add(HealStatics().HealingDef);
}

void UHealExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Healing = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(FHealStatics().HealingDef, EvaluationParameters, Healing);

	const float FinalHealing = FMath::Max(Healing, 0.f);
	if (FinalHealing > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMyAttributeSet::GetHealingAttribute(), EGameplayModOp::Additive, FinalHealing));
	}
}
