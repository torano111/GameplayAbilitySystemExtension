// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GASXBaseCharacter.generated.h"

class UGASXPawnData;
class UGASXInputConfig;
struct FGameplayTag;
struct FInputMappingContextAndPriority;

USTRUCT()
struct FInputBindHandle
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<uint32> Data;

	FInputBindHandle() { }

	FInputBindHandle(TArray<uint32> NewData)
		: Data(NewData)
	{

	}
};

/**
 * Character base for GameplayAbilitySystemExtension plugin.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXBaseCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// If this is true and a PlayerState is found, then automatically initializes AbilitySystemComponent and call InitGameplayAbilitySystem in PossessedBy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXBaseCharacter")
	bool bAutoInitGASOnPossessed;

protected:
	TWeakObjectPtr<class UGASXAbilitySystemComponent> AbilitySystemComponent;

	/** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
	UPROPERTY(EditDefaultsOnly, Category = "GASXBaseCharacter")
	TObjectPtr<const UGASXPawnData> PawnData;

	///**
	// * Input Configs that should be added to this player when initializing the input. These configs
	// * will NOT be registered with the settings because they are added at runtime. If you want the config
	// * pair to be in the settings, then add it via the GameFeatureAction_AddInputContextMapping
	// *
	// * NOTE: You should only add to this if you do not have a game feature plugin accessible to you.
	// * If you do, then use the GameFeatureAction_AddInputContextMapping instead.
	// */
	UPROPERTY(EditAnywhere, Category = "GASXBaseCharacter|Input")
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs = false;

	UPROPERTY()
	TMap<const UGASXInputConfig*, FInputBindHandle> InputData;

public:
	AGASXBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IAbilitySystemInterface interface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End of IAbilitySystemInterface interface

	virtual class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintPure, Category="GASXBaseCharacter")
	const UGASXPawnData* GetPawnData() const { return PawnData; }

	// New data can be set if the current PawnData is nullptr. Return true if new data is set.
	UFUNCTION(BlueprintCallable, Category = "GASXBaseCharacter")
	bool SetPawnData(const UGASXPawnData* InPawnData);

	// Adds mode-specific input config
	// NOTE: This does NOT bind native actions. 
	void AddAdditionalInputConfig(const UGASXInputConfig* InputConfig);

	// Removes an input config if it has been added
	void RemoveInputConfig(const UGASXInputConfig* InputConfig);

	// True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added
	UFUNCTION(BlueprintPure, Category = "GASXBaseCharacter")
	bool IsReadyToBindInputs() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Place to initialize GAS things, such as ASC, attributes, startup effects and abilities etc.
	// This should be called in PossessedBy for hero characters, and in BeginPlay for AI controlled characters. NewPlayerState can be nullptr for AI controlled characters.
	virtual void InitGameplayAbilitySystem(AActor* InOwnerActor, AActor* InAvatarActor, class AGASXPlayerState* NewPlayerState = nullptr);

	virtual void InitializePlayerInput();

	// Event to bind native actions to their corresponding tags. Use UGASXInputComponent's Bind Native Action node to bind native actions in BP graph.
	UFUNCTION(BlueprintNativeEvent, Category = "GASXBaseCharacter")
	void BindNativeActions(class UGASXInputComponent* IC, const class UGASXInputConfig* InputConfig);
	virtual void BindNativeActions_Implementation(class UGASXInputComponent* IC, const class UGASXInputConfig* InputConfig);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	////////////////////
	////////// Movement mode tags

	virtual void InitializeMovementModeTags();
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
	// Override this if you have custom movement modes. This should return mapping of your custom movement modes and corresponding tags.
	const TMap<uint8, FGameplayTag> GetCustomMovementModeTagMap() const { return TMap<uint8, FGameplayTag>();  }
};
