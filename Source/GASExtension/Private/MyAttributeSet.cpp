// Copyright 2024 Toranosuke Ichikawa

#include "MyAttributeSet.h"
#include "GameplayEffectExtension.h"

UMyAttributeSet::UMyAttributeSet()
	: Super()
{

}

void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float FinalDamage = FMath::Max(GetDamage(), 0);
		SetHealth(FMath::Clamp(GetHealth() - FinalDamage, 0.f, GetMaxHealth()));
		SetDamage(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const float FinalHealing = FMath::Max(GetHealing(), 0);
		SetHealth(FMath::Clamp(GetHealth() + FinalHealing, 0.f, GetMaxHealth()));
		SetHealing(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// MaxHealth may become less than Health
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		// MaxMana may become less than Mana
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
}

void UMyAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		// 0 <= Health <= MaxHealth
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// 1 <= Max MaxHealth
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetManaAttribute())
	{
		// 0 <= Mana <= MaxMana
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		// 1 <= Max MaxMana
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
