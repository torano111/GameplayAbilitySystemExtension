// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "MyGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class GASEXTENSION_API UMyGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
	
protected:
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override
	{
		return false;	// stop initial async loading of all gameplaycues on startup.
	}
};
