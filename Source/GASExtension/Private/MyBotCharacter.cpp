// Copyright 2024 Toranosuke Ichikawa

#include "MyBotCharacter.h"
#include "GASXAbilitySystemComponent.h"
#include "GASXPawnComponent.h"

AMyBotCharacter::AMyBotCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// We want to initialize GAS on BeginPlay manually.
	bAutoInitGASOnPossessed = false;

	// Create ability system component. 
	// This project doesn't support multiplayer for now.
	ASCReference = CreateDefaultSubobject<UGASXAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASCReference->SetIsReplicated(false);
	AbilitySystemComponent = ASCReference;

	MyAttributeSetReference = CreateDefaultSubobject<UMyAttributeSet>(TEXT("MyAttributeSet"));
	MyAttributeSet = MyAttributeSetReference;
}

void AMyBotCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(AbilitySystemComponent.IsValid(), TEXT("Failed to initialize GASXPawnComponent because AbilitySystemComponent is not valid.")))
	{
		check(GetGASXPawnComponent());
		GetGASXPawnComponent()->Initialize(AbilitySystemComponent.Get(), this, nullptr);
	}
}
