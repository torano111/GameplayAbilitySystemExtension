// Copyright 2024 Toranosuke Ichikawa

#include "GASXGameplayTags.h"
#include "GASXMacroDefinitions.h"

namespace GASXGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ExtensionEvent_BindInputsNow, "GASXNativeTag.ExtensionEvent.BindInputsNow", "GameFrameworkComponent Extension Event sent when ability inputs are ready to bind");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ExtensionEvent_AbilityReady, "GASXNativeTag.ExtensionEvent.AbilityReady", "GameFrameworkComponent Extension Event sent after abilities are granted");

	// These are mapped to the movement modes inside MovementModeTagMap
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Walking, "GASXNativeTag.Movement.Mode.Walking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_NavWalking, "GASXNativeTag.Movement.Mode.NavWalking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Falling, "GASXNativeTag.Movement.Mode.Falling", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Swimming, "GASXNativeTag.Movement.Mode.Swimming", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Flying, "GASXNativeTag.Movement.Mode.Flying", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Custom, "GASXNativeTag.Movement.Mode.Custom", "This is invalid and should be replaced with custom tags.  See LyraGameplayTags::CustomMovementModeTagMap.");

	// Mapping of ACharacter movement modes and corresponding tags.
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_Walking, Movement_Mode_Walking },
		{ MOVE_NavWalking, Movement_Mode_NavWalking },
		{ MOVE_Falling, Movement_Mode_Falling },
		{ MOVE_Swimming, Movement_Mode_Swimming },
		{ MOVE_Flying, Movement_Mode_Flying },
		{ MOVE_Custom, Movement_Mode_Custom }
	};
}

FGameplayTag UGASXGameplayTagsHelper::FindTagByString(const FString& TagString, bool bMatchPartialString)
{
	const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

	if (!Tag.IsValid() && bMatchPartialString)
	{
		FGameplayTagContainer AllTags;
		Manager.RequestAllGameplayTags(AllTags, true);

		for (const FGameplayTag& TestTag : AllTags)
		{
			if (TestTag.ToString().Contains(TagString))
			{
				UE_LOG(LogGASX, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
				Tag = TestTag;
				break;
			}
		}
	}

	return Tag;
}

FGameplayTag UGASXGameplayTagsHelper::Conv_MovementModeToGameplayTag(EMovementMode MovementMode)
{
	const FGameplayTag* MovementModeTag = GASXGameplayTags::MovementModeTagMap.Find(MovementMode);
	check(MovementModeTag && MovementModeTag->IsValid());
	return *MovementModeTag;
}

EMovementMode UGASXGameplayTagsHelper::Conv_GameplayTagToMovementMode(FGameplayTag MovementModeTag)
{
	if (MovementModeTag.IsValid())
	{
		for (const auto& TagMapping : GASXGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value == MovementModeTag)
			{
				return static_cast<EMovementMode>(TagMapping.Key);
			}
		}
	}
	UE_LOG(LogGASX, Warning, TEXT("Conv_TagToMovementMode: Invalid or unrecognized MovementModeTag: %s"), *MovementModeTag.ToString());
	return EMovementMode::MOVE_None;
}

FName UGASXGameplayTagsHelper::GetGASXExtensionEventName(FGameplayTag InGameplayTag)
{
	if (ensureMsgf(InGameplayTag.IsValid(), TEXT("GetGASXExtensionEventName: InGameplayTag is NOT valid.")))
	{
		return FName(*Conv_GameplayTagToString(InGameplayTag));
	}
	return FName();
}
