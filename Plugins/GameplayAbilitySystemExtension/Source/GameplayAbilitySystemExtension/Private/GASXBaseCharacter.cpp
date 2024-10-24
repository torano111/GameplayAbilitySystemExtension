// Copyright 2024 Toranosuke Ichikawa

#include "GASXBaseCharacter.h"
#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"

// Sets default values
AGASXBaseCharacter::AGASXBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGASXBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGASXBaseCharacter::PossessedBy(AController* NewController)
{
	// Initialize GAS before ReceivePossessed (BP event Possessed)
	AGASXPlayerState* PS = NewController ? Cast<AGASXPlayerState>(NewController->PlayerState) : nullptr;
	InitializeGAS(NewController, PS);

	Super::PossessedBy(NewController);
}

// Called every frame
void AGASXBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGASXBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AGASXBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UGASXAbilitySystemComponent* AGASXBaseCharacter::GetGASXAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

