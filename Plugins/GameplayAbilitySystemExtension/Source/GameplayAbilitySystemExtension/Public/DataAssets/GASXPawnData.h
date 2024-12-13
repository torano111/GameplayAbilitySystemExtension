// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GASXPawnData.generated.h"

class APawn;
class UGASXAbilitySet;
class UGASXAbilityTagRelationshipMap;
class UGASXInputConfig;
class UObject;


/**
 * UGASXPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "GASX Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UGASXPawnData(const FObjectInitializer& ObjectInitializer)
	{
		PawnClass = nullptr;
		InputConfig = nullptr;
	}

public:

	// Class to instantiate for this pawn (should usually derive from AGASXBaseCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASXPawnData|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASXPawnData|Abilities")
	TArray<TObjectPtr<UGASXAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASXPawnData|Abilities")
	TObjectPtr<UGASXAbilityTagRelationshipMap> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASXPawnData|Input")
	TObjectPtr<UGASXInputConfig> InputConfig;
};
