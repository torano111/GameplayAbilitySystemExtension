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

	if (AbilitySystemComponent.IsValid())
	{
		for (const auto GAClass : StartupAbilities)
		{
			auto GA = GAClass->GetDefaultObject<UGASXGameplayAbility>();
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GA, 0, -1, this));
		}

		auto FXContext = AbilitySystemComponent->MakeEffectContext();
		FXContext.AddSourceObject(this);
		for (const auto GEClass : StartupEffects)
		{
			auto GESpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GEClass, 0, FXContext);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
		}
	}
}
