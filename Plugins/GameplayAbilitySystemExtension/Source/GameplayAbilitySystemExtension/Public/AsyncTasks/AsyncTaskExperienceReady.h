// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncTaskExperienceReady.generated.h"

class AGameStateBase;
class UWorld;
class UGASXExperienceDefinition;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExperienceReadyAsyncDelegate);

/**
 * Asynchronously waits for the game state to be ready and valid and then calls the OnReady event.  Will call OnReady
 * immediately if the game state is valid already.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UAsyncTaskExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	// Waits for the experience to be determined and loaded
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncTaskExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

	virtual void Activate() override;

public:

	// Called when the experience has been determined and is ready/loaded
	UPROPERTY(BlueprintAssignable)
	FExperienceReadyAsyncDelegate OnReady;

private:
	void Step1_ListenToExperienceLoading(AGameModeBase* GameMode);
	void Step2_HandleExperienceLoaded(const UGASXExperienceDefinition* CurrentExperience);
	void Step3_BroadcastReady();

	TWeakObjectPtr<UWorld> WorldPtr;
};
