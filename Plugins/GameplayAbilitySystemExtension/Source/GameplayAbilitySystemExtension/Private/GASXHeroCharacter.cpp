// Copyright 2024 Toranosuke Ichikawa

#include "GASXHeroCharacter.h"
#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"

AGASXHeroCharacter::AGASXHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AGASXHeroCharacter::InitializeGAS(AController* NewController, AGASXPlayerState* NewPlayerState)
{
	if (NewPlayerState)
	{
		AbilitySystemComponent = NewPlayerState->GetGASXAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(NewPlayerState, this);
	}
}
