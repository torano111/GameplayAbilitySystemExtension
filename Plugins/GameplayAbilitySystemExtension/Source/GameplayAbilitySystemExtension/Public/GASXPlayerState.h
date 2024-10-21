// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GASXDataTypes.h"
#include "GASXPlayerState.generated.h"

class UGASXAbilitySystemComponent;

/**
 * PlayerState base for GameplayAbilitySystemExtension plugin.
 * By default, this class creates and holds AbilitySystemComponent(ASC). If you want ACharacter subclass to hold it instead, then make subclasses of AGASXPlayerState and AGASXHeroCharacter, and do that in constructor.
 * Currently, this plugin doesn't support multiplayer. Thus, ASC's replicate is disabled.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class UGASXAbilitySystemComponent* AbilitySystemComponent;

public:
	AGASXPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IAbilitySystemInterface interface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End of IAbilitySystemInterface interface

	virtual class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const;
};
