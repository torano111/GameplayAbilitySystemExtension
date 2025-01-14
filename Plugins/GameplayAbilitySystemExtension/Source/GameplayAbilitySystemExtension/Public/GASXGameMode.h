// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ModularGameMode.h"
#include "GASXGameMode.generated.h"

class UGASXPawnData;
class UGASXExperienceDefinition;
class UGASXExperienceManagerComponent;

/**
 * Post login event, triggered when a player or bot joins the game as well as after seamless and non seamless travel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGASXGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);


/**
 * AGASXGameMode
 *
 *	The base game mode class used by this project.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	// Handles loading and managing the current gameplay experience
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGASXExperienceManagerComponent> ExperienceManagerComponent;
 
	// Delegate called on player initialization, described above 
	FOnGASXGameModePlayerInitialized OnGameModePlayerInitialized;

public:
	AGASXGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	//virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	//virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void InitGameState() override;
	virtual bool UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage) override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	//~End of AGameModeBase interface

	UFUNCTION(BlueprintCallable, Category = "GASXGameMode|Pawn")
	const UGASXPawnData* GetPawnDataForController(const AController* InController) const;

	// Agnostic version of PlayerCanRestart that can be used for both player bots and players
	virtual bool ControllerCanRestart(AController* Controller);

	// Restart (respawn) the specified player or bot next frame
	// - If bForceReset is true, the controller will be reset this frame (abandoning the currently possessed pawn, if any)
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

protected:
	virtual void HandleMatchAssignmentIfNotExpectingOne();
	virtual void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UGASXExperienceDefinition* CurrentExperience);
};
