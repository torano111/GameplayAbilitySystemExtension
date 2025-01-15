// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GASXDataTypes.h"
#include "GASXExperienceManagerComponent.generated.h"

class UGASXExperienceDefinition;

namespace UE::GameFeatures { struct FResult; }

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGASXExperienceLoaded, const UGASXExperienceDefinition* /*Experience*/);

/**
 *	An actor component to manage experience, such as loading it based on UGASXExperienceDefinition.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXExperienceManagerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<const UGASXExperienceDefinition> CurrentExperience;

	UPROPERTY(BlueprintReadOnly, Category = "GASXExperienceManagerComponent")
	EGASXExperienceLoadState LoadState = EGASXExperienceLoadState::Unloaded;

	TArray<FString> GameFeaturePluginURLs;
	int32 NumGameFeaturePluginsLoading = 0;
	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/**
	 * Delegate called when the experience has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 */
	FOnGASXExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnGASXExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnGASXExperienceLoaded OnExperienceLoaded_LowPriority;
	
public:
	UGASXExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface
	
	// Tries to set the current experience, either a UI or gameplay one.
	// This loads the experience definition and starts loading experience.
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	UFUNCTION(BlueprintPure, Category = "GASXExperienceManagerComponent")
	const UGASXExperienceDefinition* GetCurrentExperienceChecked() const;

	// Returns true if the experience is fully loaded
	UFUNCTION(BlueprintCallable, Category = "GASXExperienceManagerComponent")
	bool IsExperienceLoaded() const;

	// Ensures the delegate is called once the experience has been loaded,
	// before others are called.
	// However, if the experience has already loaded, calls the delegate immediately.
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnGASXExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnGASXExperienceLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnGASXExperienceLoaded::FDelegate&& Delegate);

protected:
	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();

	/**
	 * Functions to load experiences. There are 4 loading stages.
	 */
	// This handles loading stage 1: Load experience definition, associated assets, and asset bundles.
	virtual void StartExperienceLoad();
	// This handles loading stage 2: Load and activate game feature plugins.
	virtual void OnExperienceLoadComplete();
	// Callback to check if all of the game features are loaded.
	virtual void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	// This handles loading stage 3 & 4: Chaos testing and execute game feature actions.
	virtual void OnExperienceFullLoadCompleted();
};
