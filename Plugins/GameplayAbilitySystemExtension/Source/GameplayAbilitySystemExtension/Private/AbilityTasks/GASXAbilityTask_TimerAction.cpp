// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilityTasks/GASXAbilityTask_TimerAction.h"
#include "TimerManager.h"
#include "Engine/World.h"

UGASXAbilityTask_TimerAction::UGASXAbilityTask_TimerAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGASXAbilityTask_TimerAction::PerformAction()
{
	OnPerformAction.Broadcast();

	if (!bLooping) EndTask();
}

UGASXAbilityTask_TimerAction* UGASXAbilityTask_TimerAction::TimerAction(UGameplayAbility* OwningAbility, float Time, bool bLooping, bool bMaxOncePerFrame, float InitialStartDelay)
{
	UGASXAbilityTask_TimerAction* MyObj = NewAbilityTask<UGASXAbilityTask_TimerAction>(OwningAbility);

	MyObj->Time = Time;
	MyObj->bLooping = bLooping;
	MyObj->bMaxOncePerFrame = bMaxOncePerFrame;
	MyObj->InitialStartDelay = InitialStartDelay;

	return MyObj;
}

void UGASXAbilityTask_TimerAction::Activate()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(TimerHandle_PerformAction, this, &UGASXAbilityTask_TimerAction::PerformAction, Time, FTimerManagerTimerParameters{ .bLoop = bLooping, .bMaxOncePerFrame = bMaxOncePerFrame, .FirstDelay = Time + InitialStartDelay });
	}
}

void UGASXAbilityTask_TimerAction::PauseTimer()
{
	if (TimerHandle_PerformAction.IsValid())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().PauseTimer(TimerHandle_PerformAction);
		}
	}
}

void UGASXAbilityTask_TimerAction::UnPauseTimer()
{
	if (TimerHandle_PerformAction.IsValid())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().UnPauseTimer(TimerHandle_PerformAction);
		}
	}
}

bool UGASXAbilityTask_TimerAction::IsTimerActive()
{
	if (TimerHandle_PerformAction.IsValid())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			return World->GetTimerManager().IsTimerActive(TimerHandle_PerformAction);
		}
	}
	return false;
}

bool UGASXAbilityTask_TimerAction::IsTimerPaused()
{
	if (TimerHandle_PerformAction.IsValid())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			return World->GetTimerManager().IsTimerPaused(TimerHandle_PerformAction);
		}
	}
	return false;
}

float UGASXAbilityTask_TimerAction::GetTimerElapsedTime()
{
	if (TimerHandle_PerformAction.IsValid())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			return World->GetTimerManager().GetTimerElapsed(TimerHandle_PerformAction);
		}
	}
	return 0.f;
}

float UGASXAbilityTask_TimerAction::GetTimerRemainingTime()
{
	if (TimerHandle_PerformAction.IsValid())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			return World->GetTimerManager().GetTimerRemaining(TimerHandle_PerformAction);
		}
	}
	return 0.f;
}

bool UGASXAbilityTask_TimerAction::IsValidTimerHandle()
{
	UWorld* World = GetWorld();
	if (World)
	{
		return World->GetTimerManager().TimerExists(TimerHandle_PerformAction);
	}
	return false;
}

void UGASXAbilityTask_TimerAction::ClearAndInvalidate()
{
	if (!IsFinished()) EndTask();
}

void UGASXAbilityTask_TimerAction::OnDestroy(bool AbilityIsEnding)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(TimerHandle_PerformAction);
	}

	Super::OnDestroy(AbilityIsEnding);
}

FString UGASXAbilityTask_TimerAction::GetDebugString() const
{
	return FString::Printf(TEXT("Timer. Time: %.2f. bLooping: %s. bMaxOncePerFrame: %s. InitialStartDelay: %.2f.")
		, Time
		, *FString(bLooping ? "true" : "false")
		, *FString(bMaxOncePerFrame ? "true" : "false")
		, InitialStartDelay
	);
}

