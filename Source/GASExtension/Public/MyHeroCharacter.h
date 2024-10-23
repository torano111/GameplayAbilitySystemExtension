// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXHeroCharacter.h"
#include "MyHeroCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GASEXTENSION_API AMyHeroCharacter : public AGASXHeroCharacter
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<class UMyAttributeSet> MyAttributeSet;
	
public:
	AMyHeroCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "MyHeroCharacter")
	class UMyAttributeSet* GetMyAttributeSet() const;

protected:
	virtual void InitializeGAS(AController* NewController, class AGASXPlayerState* NewPlayerState) override;
};
