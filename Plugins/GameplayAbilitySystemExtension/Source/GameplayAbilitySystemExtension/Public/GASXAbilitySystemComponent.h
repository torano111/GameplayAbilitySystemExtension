// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GASXAbilitySystemComponent.generated.h"

/**
 * AbilitySystemComponent for GameplayAbilitySystemExtension plugin.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UGASXAbilitySystemComponent(const FObjectInitializer& ObjectInitializer);
};
