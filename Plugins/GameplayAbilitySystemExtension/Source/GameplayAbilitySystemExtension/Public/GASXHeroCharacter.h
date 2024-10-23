// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXBaseCharacter.h"
#include "GASXHeroCharacter.generated.h"

/**
 * Hero(Player) character base
 * Currently, this doesn't work with multiplayer.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXHeroCharacter : public AGASXBaseCharacter
{
	GENERATED_BODY()

public:
	AGASXHeroCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void InitializeGAS(AController* NewController, class AGASXPlayerState* NewPlayerState) override;
};
