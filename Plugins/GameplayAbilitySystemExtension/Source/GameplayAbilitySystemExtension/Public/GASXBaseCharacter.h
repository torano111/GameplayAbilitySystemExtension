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

public:
	// If this is true and a PlayerState is found, then automatically initializes AbilitySystemComponent and call InitGameplayAbilitySystem in PossessedBy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXBaseCharacter")
	bool bAutoInitGASOnPossessed;

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

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Place to initialize GAS things, such as ASC, attributes, startup effects and abilities etc.
	// This should be called in PossessedBy for hero characters, and in BeginPlay for AI controlled characters. NewPlayerState can be nullptr for AI controlled characters.
	virtual void InitGameplayAbilitySystem(AActor* InOwnerActor, AActor* InAvatarActor, class AGASXPlayerState* NewPlayerState = nullptr);
};
