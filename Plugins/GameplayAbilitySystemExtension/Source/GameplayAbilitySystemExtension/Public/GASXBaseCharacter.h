// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GASXBaseCharacter.generated.h"

/**
 * Character base for GameplayAbilitySystemExtension plugin.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<class UGASXAbilitySystemComponent> AbilitySystemComponent;

public:
	AGASXBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IAbilitySystemInterface interface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End of IAbilitySystemInterface interface

	virtual class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
