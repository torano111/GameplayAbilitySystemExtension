// Copyright 2024 Toranosuke Ichikawa

#include "GASXAttributeSet.h"
#include "GASXAbilitySystemComponent.h"

UGASXAttributeSet::UGASXAttributeSet()
	: Super()
{

}

UWorld* UGASXAttributeSet::GetWorld() const
{
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

UGASXAbilitySystemComponent* UGASXAttributeSet::GetGASXAbilitySystemComponent() const
{
	return Cast<UGASXAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
