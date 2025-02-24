// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsyncTasks/AsyncTaskExperienceReady.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GASXGameMode.h"
#include "Experience/GASXExperienceManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncTaskExperienceReady)

UAsyncTaskExperienceReady::UAsyncTaskExperienceReady(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncTaskExperienceReady* UAsyncTaskExperienceReady::WaitForExperienceReady(UObject* InWorldContextObject)
{
	UAsyncTaskExperienceReady* Action = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Action = NewObject<UAsyncTaskExperienceReady>();
		Action->WorldPtr = World;
		Action->RegisterWithGameInstance(World);
	}

	return Action;
}

void UAsyncTaskExperienceReady::Activate()
{
	if (UWorld* World = WorldPtr.Get())
	{
		auto GameMode = UGameplayStatics::GetGameMode(World);
		check(GameMode);
		Step1_ListenToExperienceLoading(GameMode);
	}
	else
	{
		// No world so we'll never finish naturally
		SetReadyToDestroy();
	}
}

void UAsyncTaskExperienceReady::Step1_ListenToExperienceLoading(AGameModeBase* GameMode)
{
	check(GameMode);
	UGASXExperienceManagerComponent* ExperienceComponent = GameMode->FindComponentByClass<UGASXExperienceManagerComponent>();
	if (ensureMsgf(ExperienceComponent, TEXT("UAsyncTaskExperienceReady: The game mode doesn't have GASXExperienceManagerComponent.")))
	{
		if (ExperienceComponent->IsExperienceLoaded())
		{
			UWorld* World = GameMode->GetWorld();
			check(World);

			// The experience happened to be already loaded, but still delay a frame to
			// make sure people don't write stuff that relies on this always being true
			//@TODO: Consider not delaying for dynamically spawned stuff / any time after the loading screen has dropped?
			//@TODO: Maybe just inject a random 0-1s delay in the experience load itself?
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::Step3_BroadcastReady));
		}
		else
		{
			ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnGASXExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::Step2_HandleExperienceLoaded));
		}
	}
	else
	{
		// No experience manager so we'll never finish naturally
		SetReadyToDestroy();
	}
}

void UAsyncTaskExperienceReady::Step2_HandleExperienceLoaded(const UGASXExperienceDefinition* CurrentExperience)
{
	Step3_BroadcastReady();
}

void UAsyncTaskExperienceReady::Step3_BroadcastReady()
{
	OnReady.Broadcast();

	SetReadyToDestroy();
}

