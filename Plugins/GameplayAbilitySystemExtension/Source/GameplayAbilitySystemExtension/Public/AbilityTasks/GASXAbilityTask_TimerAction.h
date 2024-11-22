// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GASXAbilityTask_TimerAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimerActionDelegate);

/**
 * Perform actions with a timer.
 * NOTE: EndTask() MUST be called to clear The timer if bLooping = true.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXAbilityTask_TimerAction : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FTimerActionDelegate OnPerformAction;

	virtual FString GetDebugString() const override;

	void PerformAction();

	/** Start a task that repeats an action or set of actions. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGASXAbilityTask_TimerAction* TimerAction(UGameplayAbility* OwningAbility, float Time, bool bLooping, bool bMaxOncePerFrame = false, float InitialStartDelay = 0.f);

	virtual void Activate() override;

	// This just calls EndTask().
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|TimerAction")
	void ClearAndInvalidate();

	// Technically, this doesn't pause this task but only the timer handle.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|TimerAction")
	void PauseTimer();

	// Technically, this doesn't unpause this task but only the timer handle.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks|TimerAction")
	void UnPauseTimer();

	UFUNCTION(BlueprintPure, Category = "Ability|Tasks|TimerAction")
	bool IsTimerActive();

	UFUNCTION(BlueprintPure, Category = "Ability|Tasks|TimerAction")
	bool IsTimerPaused();

	UFUNCTION(BlueprintPure, Category = "Ability|Tasks|TimerAction")
	float GetTimerElapsedTime();

	UFUNCTION(BlueprintPure, Category = "Ability|Tasks|TimerAction")
	float GetTimerRemainingTime();

	// Just checks if the timer handle is valid. This doesn't check if this task is valid.
	UFUNCTION(BlueprintPure, Category = "Ability|Tasks|TimerAction")
	bool IsValidTimerHandle();

protected:
	float Time;
	bool bLooping;
	bool bMaxOncePerFrame;
	float InitialStartDelay;

	/** Handle for efficient management of PerformAction timer */
	FTimerHandle TimerHandle_PerformAction;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};
