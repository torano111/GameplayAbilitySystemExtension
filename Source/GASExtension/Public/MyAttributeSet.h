// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXAttributeSet.h"
#include "MyAttributeSet.generated.h"

/**
 * Example attribute set
 */
UCLASS()
class GASEXTENSION_API UMyAttributeSet : public UGASXAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxMana)

	// Incoming damage meta attribute. This is temporary value which will be reset after applied to health.
	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Damage)

	// Incoming healing meta attribute. This is temporary value which will be reset after applied to health.
	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Healing)
	
public:
	UMyAttributeSet();

	// UAttributeSet interface
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	// End of UAttributeSet interface

protected:
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue);
};
