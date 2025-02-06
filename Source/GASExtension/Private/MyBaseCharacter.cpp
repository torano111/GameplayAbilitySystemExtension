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

void AMyBaseCharacter::OnGASXPawnComponentInitialized(UGASXAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor, AActor* InAvatarActor, AGASXPlayerState* InPlayerState)
{	
	Super::OnGASXPawnComponentInitialized(InAbilitySystemComponent, InOwnerActor, InAvatarActor, InPlayerState);

	AMyPlayerState* PS = Cast<AMyPlayerState>(InPlayerState);
	if (PS)
	{
		MyAttributeSet = PS->GetMyAttributeSet();
	}
}
