// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXPlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GASEXTENSION_API AMyPlayerState : public AGASXPlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UMyAttributeSet* MyAttributeSet;
	
public:
	AMyPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category="MyPlayerState")
	class UMyAttributeSet* GetMyAttributeSet() const;
};
