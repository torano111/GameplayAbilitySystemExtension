// Copyright 2024 Toranosuke Ichikawa

#include "GASXGameplayTags.h"
#include "GASXMacroDefinitions.h"

namespace GASXGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ExtensionEvent_BindInputsNow, "GASXNativeTag.ExtensionEvent.BindInputsNow", "GameFrameworkComponent Extension Event sent when ability inputs are ready to bind");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ExtensionEvent_AbilityReady, "GASXNativeTag.ExtensionEvent.AbilityReady", "GameFrameworkComponent Extension Event sent after abilities are granted");
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

FName UGASXGameplayTagsHelper::GetGASXExtensionEventName(FGameplayTag InGameplayTag)
{
	if (ensureMsgf(InGameplayTag.IsValid(), TEXT("GetGASXExtensionEventName: InGameplayTag is NOT valid.")))
	{
		return FName(*Conv_GameplayTagToString(InGameplayTag));
	}
	return FName();
}
