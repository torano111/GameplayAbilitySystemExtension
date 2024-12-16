// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "GASXAbilityTagRelationshipMap.generated.h"

class UObject;

/** Struct that defines the relationship between different ability tags */
USTRUCT()
struct GAMEPLAYABILITYSYSTEMEXTENSION_API FAbilityTagRelationship
{
	GENERATED_BODY()

	// If an ability has this tag as one of AbilityTags, then CancelAbilitiesWithTag, BlockAbilitiesWithTag, ActivationRequiredTags and ActivationBlockedTags will be added to it.
	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;

	// Abilities with these tags are cancelled when the ability with AbilityTag is executed
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer CancelAbilitiesWithTag;

	// Abilities with these tags are blocked while the ability with AbilityTag is active
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer BlockAbilitiesWithTag;

	// The ability with AbilityTag can only be activated if the activating actor/component has all of these tags
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	// The ability with AbilityTag is blocked if the activating actor/component has any of these tags.
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};


/** Mapping of how ability tags block or cancel other abilities */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXAbilityTagRelationshipMap : public UDataAsset
{
	GENERATED_BODY()

private:
	/** The list of relationships between different gameplay tags (which ones block or cancel others) */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (TitleProperty = "AbilityTag"))
	TArray<FAbilityTagRelationship> AbilityTagRelationships;

public:
	/** Given a set of ability tags, parse the tag relationship and fill out tags to block and cancel */
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	/** Given a set of ability tags, add additional required and blocking tags */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	/** Returns true if the specified ability tags are canceled by the passed in action tag */
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};
