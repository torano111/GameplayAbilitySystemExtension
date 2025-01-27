// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "NativeGameplayTags.h"
#include "GASXGameplayTags.generated.h"

// Native tags for this plugin.
namespace GASXGameplayTags
{
    // Tags for GameFrameworkComponent Extension Events
	GAMEPLAYABILITYSYSTEMEXTENSION_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExtensionEvent_BindInputsNow);
	GAMEPLAYABILITYSYSTEMEXTENSION_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExtensionEvent_AbilityReady);
}

UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXGameplayTagsHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (FGameplayTag)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
    static FString Conv_GameplayTagToString(const FGameplayTag& InGameplayTag) { return InGameplayTag.ToString(); }

    UFUNCTION(BlueprintCallable, Category = "GASXGameplayTags")
    static FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

protected:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "GASXExtensionEventName", Categories = "GASXNativeTag.ExtensionEvent"), Category = "GASXGameplayTags")
    static FName GetGASXExtensionEventName(FGameplayTag InGameplayTag);
};
