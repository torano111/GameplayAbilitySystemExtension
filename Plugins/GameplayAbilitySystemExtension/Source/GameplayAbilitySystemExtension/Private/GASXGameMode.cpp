// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASXGameMode.h"
#include "GASXMacroDefinitions.h"
#include "Experience/GASXExperienceDefinition.h"
#include "Experience/GASXUserFacingExperienceDefinition.h"
#include "Experience/GASXExperienceManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "GASXPlayerState.h"
#include "DataAssets/GASXPawnData.h"
#include "GASXBaseCharacter.h"
#include "GASXWorldSettings.h"
#include "GASXPluginSettings.h"

AGASXGameMode::AGASXGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExperienceManagerComponent = CreateDefaultSubobject<UGASXExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void AGASXGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Wait for the next frame to give time to initialize startup settings
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

UClass* AGASXGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UGASXPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AGASXGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (AGASXBaseCharacter* GASXBaseCharacter = Cast<AGASXBaseCharacter>(SpawnedPawn))
			{
				if (const UGASXPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					GASXBaseCharacter->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogGASX, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogGASX, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogGASX, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool AGASXGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

void AGASXGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

bool AGASXGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool AGASXGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	return true;
}

void AGASXGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnGASXExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

bool AGASXGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

void AGASXGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void AGASXGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);
			}
			else
			{
				UE_LOG(LogGASX, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogGASX, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}

void AGASXGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
	//else if (ALyraPlayerBotController* BotController = Cast<ALyraPlayerBotController>(Controller))
	//{
	//	GetWorldTimerManager().SetTimerForNextTick(BotController, &ALyraPlayerBotController::ServerRestartController);
	//}
}

const UGASXPawnData* AGASXGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const AGASXPlayerState* PS = InController->GetPlayerState<AGASXPlayerState>())
		{
			if (const UGASXPawnData* PawnData = PS->GetPawnData())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current experience
	check(ExperienceManagerComponent);
	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UGASXExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// Experience is loaded and there's still no pawn data, fall back to the default for now
		//return ULyraAssetManager::Get().GetDefaultPawnData();
	}

	// Experience not loaded yet, so there is no pawn data to be had
	return nullptr;
}

void AGASXGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	// Tries to find experience to load

	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins)
	//  - URL Options override
	//  - Command Line override
	//  - World Settings
	//  - Default experience from Project Settings

	UWorld* World = GetWorld();

	// see if travel URL option provides experience.
	// If UGASXUserFacingExperienceDefinition was used to travel, then this should set ExperienceId.
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, UGASXUserFacingExperienceDefinition::ExperienceOptionName))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, UGASXUserFacingExperienceDefinition::ExperienceOptionName);
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UGASXExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// see if the command line wants to set the experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UGASXExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (AGASXWorldSettings* TypedWorldSettings = Cast<AGASXWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	// check if ExperienceId is valid
	UAssetManager& AssetManager = UAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogGASXExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}

	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		if (const UGASXPluginSettings* Settings = GetDefault<UGASXPluginSettings>())
		{
			if (!Settings->DefaultExperience.IsNull())
			{
				ExperienceId = UAssetManager::Get().GetPrimaryAssetIdForPath(Settings->DefaultExperience);
				ExperienceIdSource = TEXT("Default");
			}
		}
	}

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void AGASXGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogGASXExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		check(ExperienceManagerComponent);
		ExperienceManagerComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogGASXExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

bool AGASXGameMode::IsExperienceLoaded() const
{
	check(ExperienceManagerComponent);
	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AGASXGameMode::OnExperienceLoaded(const UGASXExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}
