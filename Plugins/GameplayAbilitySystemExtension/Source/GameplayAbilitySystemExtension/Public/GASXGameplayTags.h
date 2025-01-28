// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "NativeGameplayTags.h"
#include "GASXGameplayTags.generated.h"

// Native tags for this plugin.
namespace GASXGameplayTags
{
	// Tags for GameFrameworkComponent Extension Events
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExtensionEvent_BindInputsNow);
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExtensionEvent_AbilityReady);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	GAMEPLAYABILITYSYSTEMEXTENSION_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
	GAMEPLAYABILITYSYSTEMEXTENSION_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
}

UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXGameplayTagsHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (GameplayTag)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
	static FString Conv_GameplayTagToString(const FGameplayTag& InGameplayTag) { return InGameplayTag.ToString(); }

	UFUNCTION(BlueprintCallable, Category = "GASXGameplayTags")
	static FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To GameplayTag (MovementMode)", CompactNodeTitle = "->", BlueprintAutocast), Category = "GASXGameplayTags")
	static FGameplayTag Conv_MovementModeToGameplayTag(EMovementMode MovementMode);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To MovementMode (GameplayTag)", CompactNodeTitle = "->", BlueprintAutocast), Category = "GASXGameplayTags")
	static EMovementMode Conv_GameplayTagToMovementMode(FGameplayTag MovementModeTag);

protected:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GASXExtensionEventName", Categories = "GASXNativeTag.ExtensionEvent"), Category = "GASXGameplayTags")
	static FName GetGASXExtensionEventName(FGameplayTag InGameplayTag);
};
