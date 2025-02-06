// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GASXBaseCharacter.generated.h"

class UGASXPawnComponent;
struct FGameplayTag;

/**
 * Character base for GameplayAbilitySystemExtension plugin.
 * You don't have to use this class as base. But if you want to make your own pawn base class:
 * 1) Your pawn must have UGASXPawnComponent and UGASXAbilitySystemComponent. Initialize UGASXPawnComponent in PossessedBy() and bind inputs in SetupPlayerInputComponent().
 * 2) Your pawn must implement IAbilitySystemInterface.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXBaseCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// If this is true and a PlayerState is valid, then automatically initializes GASXPawnComponent in PossessedBy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXBaseCharacter")
	bool bAutoInitGASOnPossessed;

protected:
	UPROPERTY(VisibleAnywhere, Category = "GASXBaseCharacter")
	TObjectPtr<UGASXPawnComponent> GASXPawnComponent;

	TWeakObjectPtr<class UGASXAbilitySystemComponent> AbilitySystemComponent;

public:
	AGASXBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IAbilitySystemInterface interface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End of IAbilitySystemInterface interface

	UFUNCTION(BlueprintPure, Category = "GASXBaseCharacter")
	virtual class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure, Category = "GASXBaseCharacter")
	virtual UGASXPawnComponent* GetGASXPawnComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnGASXPawnComponentInitialized(UGASXAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor, AActor* InAvatarActor, class AGASXPlayerState* InPlayerState);

	////////////////////
	////////// Movement mode tags

	virtual void InitializeMovementModeTags();
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
	// Override this if you have custom movement modes. This should return mapping of your custom movement modes and corresponding tags.
	const TMap<uint8, FGameplayTag> GetCustomMovementModeTagMap() const { return TMap<uint8, FGameplayTag>();  }
};
