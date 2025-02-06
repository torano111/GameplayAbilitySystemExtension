// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GASXPawnComponent.generated.h"

class UGASXPawnData;
class UGASXInputConfig;
struct FGameplayTag;
class UGASXInputComponent;
struct FInputMappingContextAndPriority;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGASXPawnComponentInitializedDelegate, UGASXAbilitySystemComponent*, AbilitySystemComponent, AActor*, OwnerActor, AActor*, AvatarActor, AGASXPlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASXPawnComponentUninitializedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBindNativeActionsDelegate, UGASXInputComponent*, IC, const UGASXInputConfig*, InputConfig);

/**
 * Core component for GameplayAbilitySystemExtension plugin.
 * This component grants abilities and binds them to inputs, based on PawnData, and sends extension events.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXPawnComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	// Delegate fired when this component is initialized
	UPROPERTY(BlueprintAssignable)
	FGASXPawnComponentInitializedDelegate OnInitialized;

	// Delegate fired when this component is uninitialized
	UPROPERTY(BlueprintAssignable)
	FGASXPawnComponentUninitializedDelegate OnUninitialized;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "Bind Native Actions"))
	FBindNativeActionsDelegate OnBindNativeActions;

protected:
	TWeakObjectPtr<class UGASXAbilitySystemComponent> AbilitySystemComponent;

	/** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
	UPROPERTY(VisibleAnywhere, Category = "GASXPawnComponent")
	TObjectPtr<const UGASXPawnData> PawnData;

	///**
	// * Input Configs that should be added to this player when initializing the input. These configs
	// * will NOT be registered with the settings because they are added at runtime. If you want the config
	// * pair to be in the settings, then add it via the GameFeatureAction_AddInputContextMapping
	// *
	// * NOTE: You should only add to this if you do not have a game feature plugin accessible to you.
	// * If you do, then use the GameFeatureAction_AddInputContextMapping instead.
	// */
	UPROPERTY(EditAnywhere, Category = "GASXPawnComponent|Input")
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	UPROPERTY(EditAnywhere, Category = "GASXPawnComponent|ExtensionEvent")
	FGameplayTag ExtensionEventTag_BindInputsNow;

	UPROPERTY(EditAnywhere, Category = "GASXPawnComponent|ExtensionEvent")
	FGameplayTag ExtensionEventTag_AbilityReady;

	bool bReadyToBindInputs;
	bool bAbilityReady;
	bool bInitialized;

	struct FInputBindHandle
	{
	public:
		TArray<uint32> Data;

		FInputBindHandle() {}

		FInputBindHandle(TArray<uint32> NewData)
			: Data(NewData)
		{

		}
	};
	TMap<const UGASXInputConfig*, FInputBindHandle> InputData;

public:
	// Sets default values for this component's properties
	UGASXPawnComponent(const FObjectInitializer& ObjectInitializer);

	// Initializes this component. For a player pawn owner, this should be called by PossessedBy(). For AI controlled owner, BeginPlay().
	// This is also a place to initialize GAS things, such as ASC, attributes, startup effects and abilities etc.
	// @param InGASXAbilitySystemComponent	AbilitySystemComponent of Owner
	// @param InGASOwnerActor	new OwnerActor of the given GASXAbilitySystemComponent
	// @param NewPlayerState	This can be nullptr for AI controlled pawn owner.
	virtual void Initialize(UGASXAbilitySystemComponent* InGASXAbilitySystemComponent, AActor* InGASOwnerActor, class AGASXPlayerState* NewPlayerState = nullptr);
	
	// Uninitializes this component.
	virtual void Uninitialize();

	// Adds DefaultInputMappings and binds PawnData's InputConfig.
	// Owner must have PlayerController.
	virtual void InitializePlayerInput(UGASXInputComponent* PlayerInputComponent);

	virtual class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const { return AbilitySystemComponent.Get(); }

	UFUNCTION(BlueprintPure, Category = "GASXPawnComponent")
	const UGASXPawnData* GetPawnData() const { return PawnData; }

	// New data can be set if the current PawnData is nullptr. Return true if new data is set.
	UFUNCTION(BlueprintCallable, Category = "GASXPawnComponent")
	bool SetPawnData(const UGASXPawnData* InPawnData);

	// Adds mode-specific input config
	// NOTE: This does NOT bind native actions. 
	bool AddAdditionalInputConfig(const UGASXInputConfig* InputConfig);

	// Removes an input config if it has been added
	void RemoveInputConfig(const UGASXInputConfig* InputConfig);

	// True if player input bindings have been applied, will never be true for non - players
	UFUNCTION(BlueprintPure, Category = "GASXPawnComponent")
	bool IsReadyToBindInputs() const { return bReadyToBindInputs; }

	// True if AbilitySets and TagRelationshipMapping have been applied.
	UFUNCTION(BlueprintPure, Category = "GASXPawnComponent")
	bool IsAbilityReady() const { return bAbilityReady; }

	// True if GameplayAbilitySystem is initialized.
	UFUNCTION(BlueprintPure, Category = "GASXPawnComponent")
	bool IsInitialized() const { return bInitialized; }

	// Returns the pawn extension component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Lyra|Pawn")
	static UGASXPawnComponent* FindGASXPawnComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UGASXPawnComponent>() : nullptr); }


protected:
	// Called whenever this actor is being removed from a level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GrantAbilities();

	// Override this to bind native actions. For BP, use OnBindNativeActions delegete.
	virtual void BindNativeActions(class UGASXInputComponent* IC, const class UGASXInputConfig* InputConfig);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
};
