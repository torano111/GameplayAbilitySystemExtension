// Copyright 2024 Toranosuke Ichikawa

#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"

AGASXPlayerState::AGASXPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create ability system component. 
	// This project doesn't support multiplayer for now.
	AbilitySystemComponent = CreateDefaultSubobject<UGASXAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
}

UAbilitySystemComponent* AGASXPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGASXAbilitySystemComponent* AGASXPlayerState::GetGASXAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
