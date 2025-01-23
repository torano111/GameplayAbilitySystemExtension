// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXExtensionEvents.generated.h"

/**
 * Names of UGameFrameworkComponentManager extension events for this plugin.
 */
struct FGASXExtensionEvents
{
    // Event sent when ability inputs are ready to bind
    static const FName NAME_BindInputsNow;

    // Event sent after abilities are granted
    static const FName NAME_AbilityReady;
};

UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXExtensionEventsLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "GASXExtensionEvents")
    static FName ExtensionEventName_BindInputsNow() { return FGASXExtensionEvents::NAME_BindInputsNow; }

    UFUNCTION(BlueprintPure, Category = "GASXExtensionEvents")
    static FName ExtensionEventName_AbilityReady() { return FGASXExtensionEvents::NAME_AbilityReady; }
};