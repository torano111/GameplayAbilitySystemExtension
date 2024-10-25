// Copyright 2024 Toranosuke Ichikawa

#include "MyBotCharacter.h"
#include "GASXAbilitySystemComponent.h"

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

	check(AbilitySystemComponent.IsValid());
	InitGameplayAbilitySystem(this, this, nullptr);
}
