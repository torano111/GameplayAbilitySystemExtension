// Copyright 2024 Toranosuke Ichikawa

#include "GASXPawnComponent.h"
#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/GASXPawnData.h"
#include "DataAssets/GASXAbilitySet.h"
#include "GASXInputComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GASXGameplayTags.h"
#include "GASXDataTypes.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "GASXAssetManager.h"
#include "GASXGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

UGASXPawnComponent::UGASXPawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bReadyToBindInputs(false)
	, bAbilityReady(false)
	, bInitialized(false)
{
	PrimaryComponentTick.bCanEverTick = false;

	PawnData = nullptr;
	ExtensionEventTag_BindInputsNow = GASXGameplayTags::ExtensionEvent_BindInputsNow;
	ExtensionEventTag_AbilityReady = GASXGameplayTags::ExtensionEvent_AbilityReady;
}

void UGASXPawnComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Uninitialize();

	Super::EndPlay(EndPlayReason);
}

void UGASXPawnComponent::Initialize(UGASXAbilitySystemComponent* InGASXAbilitySystemComponent, AActor* InGASOwnerActor, AGASXPlayerState* NewPlayerState)
{
	check(InGASXAbilitySystemComponent);
	check(InGASOwnerActor);

	if (AbilitySystemComponent == InGASXAbilitySystemComponent) return;

	if (AbilitySystemComponent.IsValid()) Uninitialize();	// Clean up the old ability system component.

	bInitialized = true;
	AbilitySystemComponent = InGASXAbilitySystemComponent;
	AbilitySystemComponent->InitAbilityActorInfo(InGASOwnerActor, GetPawn<APawn>());

	if (!bAbilityReady && PawnData)
	{
		GrantAbilities();
	}

	OnInitialized.Broadcast(AbilitySystemComponent.Get(), AbilitySystemComponent->GetOwnerActor(), AbilitySystemComponent->GetAvatarActor(), NewPlayerState);
}

void UGASXPawnComponent::Uninitialize()
{
	if (AbilitySystemComponent.IsValid() && bInitialized)
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() == GetOwner())
		{
			AbilitySystemComponent->ClearActorInfo();
		}
		else if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}

		OnUninitialized.Broadcast();
	}

	bReadyToBindInputs = false;
	bAbilityReady = false;
	bInitialized = false;
}

void UGASXPawnComponent::InitializePlayerInput(UGASXInputComponent* PlayerInputComponent)
{
	if (bReadyToBindInputs) return;

	check(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (PawnData)
	{
		if (const UGASXInputConfig* InputConfig = PawnData->InputConfig)
		{
			for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
			{
				bool bTrackInAssetManager = false;
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				bTrackInAssetManager = true;
#endif
				UGASXAssetManager& AssetManager = UGASXAssetManager::Get();
				if (UInputMappingContext* IMC = AssetManager.GetAsset(Mapping.InputMapping, bTrackInAssetManager))	// explicitly load the asset
				{
					if (Mapping.bRegisterWithSettings)
					{
						if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
						{
							Settings->RegisterInputMappingContext(IMC);
						}

						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;
						// Actually add the config to the local player							
						Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
					}
				}
			}

			// The GASXInputComponent has some additional functions to map Gameplay Tags to an Input Action.
			if (ensureMsgf(!InputData.Contains(InputConfig), TEXT("UGASXPawnComponent::InitializePlayerInput: Canceled to bind InputConfig(%s) because InputData already contained it."), *(InputConfig->GetName())))
			{
				// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
				// be triggered directly by these input actions Triggered events. 
				TArray<uint32> BindHandles;
				PlayerInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				BindNativeActions(PlayerInputComponent, InputConfig);
				OnBindNativeActions.Broadcast(PlayerInputComponent, InputConfig);

				InputData.Add(InputConfig, FInputBindHandle(BindHandles));
			}
		}
	}

	bReadyToBindInputs = true;
	check(ExtensionEventTag_BindInputsNow.IsValid());
	const auto NAME_BindInputsNow = FName(*(ExtensionEventTag_BindInputsNow.ToString()));
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow); // For UGameFeatureAction_AddInputContextMapping
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(GetOwner(), NAME_BindInputsNow);	// For UGameFeatureAction_AddInputBinding
}


bool UGASXPawnComponent::SetPawnData(const UGASXPawnData* InPawnData)
{
	bool bNewDataSet = false;
	if (InPawnData)
	{
		if (PawnData)
		{
			UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
			return false;
		}
		PawnData = InPawnData;
		bNewDataSet = true;
	}

	if (!bAbilityReady && bInitialized && PawnData && AbilitySystemComponent.IsValid())
	{
		GrantAbilities();
	}
	return bNewDataSet;
}

void UGASXPawnComponent::GrantAbilities()
{
	if (!bAbilityReady)
	{
		check(AbilitySystemComponent.IsValid());
		if (ensure(PawnData))
		{
			for (const UGASXAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				if (AbilitySet)
				{
					AbilitySet->GiveToAbilitySystem(AbilitySystemComponent.Get(), nullptr);
				}
			}

			AbilitySystemComponent->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);

			bAbilityReady = true;
			check(ExtensionEventTag_AbilityReady.IsValid());
			const auto NAME_AbilityReady = FName(*(ExtensionEventTag_AbilityReady.ToString()));
			UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(AbilitySystemComponent->GetOwnerActor(), NAME_AbilityReady);
		}
	}
}

bool UGASXPawnComponent::AddAdditionalInputConfig(const UGASXInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);
	if (const APlayerController* PC = GetController<APlayerController>())
	{
		if (const ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				UGASXInputComponent* IC = Pawn->FindComponentByClass<UGASXInputComponent>();
				if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGASXInputComponent or a subclass of it.")))
				{
					if (ensureMsgf(!InputData.Contains(InputConfig), TEXT("UGASXPawnComponent::AddAdditionalInputConfig: Canceled to bind InputConfig(%s) because InputData already contained it."), *(InputConfig->GetName())))
					{
						IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
						InputData.Add(InputConfig, FInputBindHandle(BindHandles));
						return true;
					}
				}
			}
		}
	}
	return false;
}

void UGASXPawnComponent::RemoveInputConfig(const UGASXInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	check(Pawn);
	if (const APlayerController* PC = GetController<APlayerController>())
	{
		if (const ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				UGASXInputComponent* IC = Pawn->FindComponentByClass<UGASXInputComponent>();
				if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGASXInputComponent or a subclass of it.")))
				{
					if (InputData.Contains(InputConfig))
					{
						auto HandleData = InputData.FindAndRemoveChecked(InputConfig);
						IC->RemoveBinds(HandleData.Data);
					}
				}
			}
		}
	}
}

void UGASXPawnComponent::BindNativeActions(UGASXInputComponent* IC, const UGASXInputConfig* InputConfig)
{
	// A subclass should implement this as needed.

	// Example
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
}

void UGASXPawnComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (auto ASC = GetGASXAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void UGASXPawnComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (auto ASC = GetGASXAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

