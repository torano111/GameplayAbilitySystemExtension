// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/GASXAbilityTagRelationshipMap.h"

void UGASXAbilityTagRelationshipMap::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tags.BlockAbilitiesWithTag);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Tags.CancelAbilitiesWithTag);
			}
		}
	}
}

void UGASXAbilityTagRelationshipMap::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
			}
			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
			}
		}
	}
}

bool UGASXAbilityTagRelationshipMap::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
	// Simple iteration for now
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		if (Tags.AbilityTag == ActionTag && Tags.CancelAbilitiesWithTag.HasAny(AbilityTags))
		{
			return true;
		}
	}

	return false;
}
