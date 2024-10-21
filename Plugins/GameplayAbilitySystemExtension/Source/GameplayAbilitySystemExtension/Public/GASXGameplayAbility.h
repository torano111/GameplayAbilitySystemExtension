// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASXGameplayAbility.generated.h"

/**
 * Base GameplayAbility
 */
UCLASS(Abstract)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	// Activates this ability immediately when granted. Used for passive abilities.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool bActivateAbilityOnGranted = false;

public:
	UGASXGameplayAbility();

	/** Called when the avatar actor is set/changes */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

};
