// Copyright 2024 Toranosuke Ichikawa

#include "MyBaseCharacter.h"
#include "MyPlayerState.h"
#include "GASXAbilitySystemComponent.h"
#include "GameplayAbilities/GASXGameplayAbility.h"

AMyBaseCharacter::AMyBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UMyAttributeSet* AMyBaseCharacter::GetMyAttributeSet() const
{
	return MyAttributeSet.Get();
}

void AMyBaseCharacter::InitGameplayAbilitySystem(AActor* InOwnerActor, AActor* InAvatarActor, AGASXPlayerState* NewPlayerState)
{
	Super::InitGameplayAbilitySystem(InOwnerActor, InAvatarActor, NewPlayerState);

	AMyPlayerState* PS = Cast<AMyPlayerState>(NewPlayerState);
	if (PS)
	{
		MyAttributeSet = PS->GetMyAttributeSet();
	}
}
