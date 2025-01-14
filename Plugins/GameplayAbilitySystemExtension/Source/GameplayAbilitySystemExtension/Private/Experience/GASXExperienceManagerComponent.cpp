// Copyright Epic Games, Inc. All Rights Reserved.

#include "Experience/GASXExperienceManagerComponent.h"
#include "Experience/GASXExperienceDefinition.h"
#include "Experience/GASXExperienceActionSet.h"
#include "Engine/AssetManager.h"
#include "GASXMacroDefinitions.h"
#include "GameFeaturesSubsystemSettings.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "GameFramework/GameUserSettings.h"

namespace GASXConsoleVariables
{
	static float ExperienceLoadRandomDelayMin = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayMin(
		TEXT("gasx.chaos.ExperienceDelayLoad.MinSecs"),
		ExperienceLoadRandomDelayMin,
		TEXT("This value (in seconds) will be added as a delay of load completion of the experience (along with the random value gasx.chaos.ExperienceDelayLoad.RandomSecs)"),
		ECVF_Default);

	static float ExperienceLoadRandomDelayRange = 0.0f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayRange(
		TEXT("gasx.chaos.ExperienceDelayLoad.RandomSecs"),
		ExperienceLoadRandomDelayRange,
		TEXT("A random amount of time between 0 and this value (in seconds) will be added as a delay of load completion of the experience (along with the fixed value gasx.chaos.ExperienceDelayLoad.MinSecs)"),
		ECVF_Default);

	float GetExperienceLoadDelayDuration()
	{
		return FMath::Max(0.0f, ExperienceLoadRandomDelayMin + FMath::FRand() * ExperienceLoadRandomDelayRange);
	}
}

UGASXExperienceManagerComponent::UGASXExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGASXExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// deactivate any features this experience loaded
	//@TODO: This should be handled FILO as well
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		//if (ULyraExperienceManager::RequestToDeactivatePlugin(PluginURL))
		//{
		UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		//}
	}

	//@TODO: Ensure proper handling of a partially-loaded state too
	if (LoadState == EGASXExperienceLoadState::Loaded)
	{
		LoadState = EGASXExperienceLoadState::Deactivating;

		// Make sure we won't complete the transition prematurely if someone registers as a pauser but fires immediately
		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// Deactivate and unload the actions
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}
			};

		DeactivateListOfActions(CurrentExperience->Actions);
		for (const TObjectPtr<UGASXExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (ActionSet != nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogGASXExperience, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in GASX experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

void UGASXExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UGASXExperienceManagerComponent::OnAllActionsDeactivated()
{
	//@TODO: We actually only deactivated and didn't fully unload...
	LoadState = EGASXExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
	//@TODO:	GEngine->ForceGarbageCollection(true);
}

void UGASXExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	// Load data asset of the experience definition
	FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(ExperienceId);
	const UGASXExperienceDefinition* Experience = Cast<UGASXExperienceDefinition>(AssetPath.TryLoad());

	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	CurrentExperience = Experience;
	StartExperienceLoad();
}

const UGASXExperienceDefinition* UGASXExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EGASXExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

bool UGASXExperienceManagerComponent::IsExperienceLoaded() const
{
	return (LoadState == EGASXExperienceLoadState::Loaded) && (CurrentExperience != nullptr);
}

void UGASXExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(FOnGASXExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

void UGASXExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnGASXExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UGASXExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(FOnGASXExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

void UGASXExperienceManagerComponent::StartExperienceLoad()
{
	// Loading Stage 1: Load Experience Definition
	// Only assets that are directly referenced by the experience definition are loaded here like, for example, HUD widgets in the Add Widgets action. 
	// All other assets in game feature plugins are loaded in the next stage (OnExperienceLoadComplete() and OnGameFeaturePluginLoadComplete()).

	check(CurrentExperience != nullptr);
	check(LoadState == EGASXExperienceLoadState::Unloaded);

	UE_LOG(LogGASXExperience, Log, TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s)"), *CurrentExperience->GetPrimaryAssetId().ToString());

	LoadState = EGASXExperienceLoadState::Loading;

	UAssetManager& AssetManager = UAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	for (const TObjectPtr<UGASXExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// Load assets associated with the experience

	TArray<FName> BundlesToLoad;
	// Not used right now
	//BundlesToLoad.Add(FLyraBundles::Equipped);

	//@TODO: Centralize this client/server stuff into the LyraAssetManager
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	}

	// Not used right now
	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if (RawAssetList.Num() > 0)
	{
		RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartExperienceLoad()"));
	}

	// If both async loads are running, combine them
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// Assets were already loaded, call the delegate now
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// Not used right now
	// This set of assets gets preloaded, but we don't block the start of the experience based on it
	TSet<FPrimaryAssetId> PreloadAssetList;
	//@TODO: Determine assets to preload (but not blocking-ly)
	if (PreloadAssetList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(), BundlesToLoad, {});
	}
}

void UGASXExperienceManagerComponent::OnExperienceLoadComplete()
{
	// Loading Stage 2: Load Game Features
	// Game features are loaded and activated in this stage.

	check(LoadState == EGASXExperienceLoadState::Loading);
	check(CurrentExperience != nullptr);

	UE_LOG(LogGASXExperience, Log, TEXT("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s)"), *CurrentExperience->GetPrimaryAssetId().ToString());

	// find the URLs for our GameFeaturePlugins - filtering out dupes and ones that don't have a valid mapping
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
				else
				{
					ensureMsgf(false, TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for experience %s - fix data, ignoring for this run"), *PluginName, *Context->GetPrimaryAssetId().ToString());
				}
			}

			// 		// Add in our extra plugin
			// 		if (!CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent.IsEmpty())
			// 		{
			// 			FString PluginURL;
			// 			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent, PluginURL))
			// 			{
			// 				GameFeaturePluginURLs.AddUnique(PluginURL);
			// 			}
			// 		}
		};

	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);
	for (const TObjectPtr<UGASXExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			CollectGameFeaturePluginURLs(ActionSet, ActionSet->GameFeaturesToEnable);
		}
	}

	// Load and activate the features	
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading > 0)
	{
		LoadState = EGASXExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			//ULyraExperienceManager::NotifyOfPluginActivation(PluginURL);
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnExperienceFullLoadCompleted();
	}
}

void UGASXExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// decrement the number of plugins that are loading
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void UGASXExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	// Loading Stage 3. Chaos Testing
	// This stage is optional and disabled by default. When enabled, a random delay is added to the load time here. 
	// This can help you test your game by having staggered client readiness.

	check(LoadState != EGASXExperienceLoadState::Loaded);

	// Insert a random delay for testing (if configured)
	if (LoadState != EGASXExperienceLoadState::LoadingChaosTestingDelay)
	{
		const float DelaySecs = GASXConsoleVariables::GetExperienceLoadDelayDuration();
		if (DelaySecs > 0.0f)
		{
			FTimerHandle DummyHandle;

			LoadState = EGASXExperienceLoadState::LoadingChaosTestingDelay;
			GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::OnExperienceFullLoadCompleted, DelaySecs, /*bLooping=*/ false);

			return;
		}
	}

	// Loading Stage 4. Execute Game Feature Actions
	// Game feature actions are executed in the order as they appear in the experience definition and then each action set.

	LoadState = EGASXExperienceLoadState::ExecutingActions;

	// Execute the actions
	FGameFeatureActivatingContext Context;

	// Only apply to our specific world context if set
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action != nullptr)
				{
					//@TODO: The fact that these don't take a world are potentially problematic in client-server PIE
					// The current behavior matches systems like gameplay tags where loading and registering apply to the entire process,
					// but actually applying the results to actors is restricted to a specific world
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

	ActivateListOfActions(CurrentExperience->Actions);
	for (const TObjectPtr<UGASXExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EGASXExperienceLoadState::Loaded;

	OnExperienceLoaded_HighPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_HighPriority.Clear();

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();

	// This always runs in single player
#if !UE_SERVER
	// The Lyra sample project has ULyraSettingsLocal (its custom UGameUserSettings), and calls overrided ApplyNonResolutionSettings() here.
	// For simplicity, we just call UGameUserSettings's ApplyNonResolutionSettings() here. No custom UGameUserSettings.

	// Apply any necessary scalability settings
	//ULyraSettingsLocal::Get()->OnExperienceLoaded();
	auto GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->ApplyNonResolutionSettings();
#endif
}
