// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "MyBaseCharacter.h"
#include "MyHeroCharacter.generated.h"

/**
 * Example hero character.
 */
UCLASS()
class GASEXTENSION_API AMyHeroCharacter : public AMyBaseCharacter
{
	GENERATED_BODY()
	
public:
	AMyHeroCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
