// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXPlayerController.h"
#include "LoadingProcessInterface.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASEXTENSION_API AMyPlayerController : public AGASXPlayerController, public ILoadingProcessInterface
{
	GENERATED_BODY()
	
public:
	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface
};
