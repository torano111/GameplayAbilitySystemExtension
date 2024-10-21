// Copyright 2024 Toranosuke Ichikawa

#include "GASXHeroCharacter.h"
#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"

AGASXHeroCharacter::AGASXHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AGASXHeroCharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Log, TEXT("AGASXHeroCharacter::PossessedBy"));

	if (AGASXPlayerState* PS = Cast<AGASXPlayerState>(NewController->PlayerState))
	{
		AbilitySystemComponent = PS->GetGASXAbilitySystemComponent();
	}

	Super::PossessedBy(NewController);
}
