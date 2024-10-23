// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GASXAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Base attribute set for this plugin.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGASXAttributeSet();

	UWorld* GetWorld() const override;

	class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const;
};
