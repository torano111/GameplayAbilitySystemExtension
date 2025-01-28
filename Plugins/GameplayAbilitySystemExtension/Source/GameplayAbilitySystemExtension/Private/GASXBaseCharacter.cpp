// Copyright 2024 Toranosuke Ichikawa

#include "GASXBaseCharacter.h"
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

// Sets default values
AGASXBaseCharacter::AGASXBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAutoInitGASOnPossessed(true)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnData = nullptr;
}

bool AGASXBaseCharacter::SetPawnData(const UGASXPawnData* InPawnData)
{
	if (InPawnData)
	{
		if (PawnData)
		{
			UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
			return false;
		}
		PawnData = InPawnData;
	}
	return false;
}

// Called when the game starts or when spawned
void AGASXBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AGASXBaseCharacter::PossessedBy(AController* NewController)
{
	// Initialize GAS before ReceivePossessed (BP event Possessed) if needed
	if (bAutoInitGASOnPossessed)
	{
		AGASXPlayerState* PS = NewController ? Cast<AGASXPlayerState>(NewController->PlayerState) : nullptr;
		if (PS)
		{
			if (!AbilitySystemComponent.IsValid()) AbilitySystemComponent = PS->GetGASXAbilitySystemComponent();
			InitGameplayAbilitySystem(PS, this, PS);
		}
	}

	Super::PossessedBy(NewController);
}

void AGASXBaseCharacter::InitGameplayAbilitySystem(AActor* InOwnerActor, AActor* InAvatarActor, AGASXPlayerState* NewPlayerState)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, InAvatarActor);
		InitializeMovementModeTags();
	}
}

void AGASXBaseCharacter::InitializePlayerInput()
{
	check(InputComponent);

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
			UGASXInputComponent* IC = Cast<UGASXInputComponent>(InputComponent);
			if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGASXInputComponent or a subclass of it.")))
			{
				if (ensureMsgf(!InputData.Contains(InputConfig), TEXT("AGASXBaseCharacter::InitializePlayerInput: Canceled to bind InputConfig(%s) because InputData already contained it."), *(InputConfig->GetName())))
				{
					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					BindNativeActions(IC, InputConfig);
					InputData.Add(InputConfig, FInputBindHandle(BindHandles));
				}
			}

			check(AbilitySystemComponent.IsValid());
			for (const UGASXAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				if (AbilitySet)
				{
					AbilitySet->GiveToAbilitySystem(AbilitySystemComponent.Get(), nullptr);
				}
			}

			if (ensure(PawnData))
			{
				AbilitySystemComponent->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
			}
		}
	}

	bReadyToBindInputs = true;

	const auto NAME_AbilityReady = FName(*(GASXGameplayTags::ExtensionEvent_AbilityReady.GetTag().ToString()));
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_AbilityReady);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_AbilityReady);

	const auto NAME_BindInputsNow = FName(*(GASXGameplayTags::ExtensionEvent_BindInputsNow.GetTag().ToString()));
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_BindInputsNow);
}

void AGASXBaseCharacter::BindNativeActions_Implementation(UGASXInputComponent* IC, const UGASXInputConfig* InputConfig)
{
	// A subclass should implement this as needed.

	// Example
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
	//IC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
}

void AGASXBaseCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (auto ASC = GetGASXAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AGASXBaseCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (auto ASC = GetGASXAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

void AGASXBaseCharacter::AddAdditionalInputConfig(const UGASXInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	UGASXInputComponent* IC = Cast<UGASXInputComponent>(InputComponent);
	if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGASXInputComponent or a subclass of it.")))
	{
		if (ensureMsgf(!InputData.Contains(InputConfig), TEXT("AGASXBaseCharacter::AddAdditionalInputConfig: Canceled to bind InputConfig(%s) because InputData already contained it."), *(InputConfig->GetName())))
		{
			IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
			InputData.Add(InputConfig, FInputBindHandle(BindHandles));
		}
	}
}

void AGASXBaseCharacter::RemoveInputConfig(const UGASXInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	UGASXInputComponent* IC = Cast<UGASXInputComponent>(InputComponent);
	if (ensureMsgf(IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UGASXInputComponent or a subclass of it.")))
	{
		if (InputData.Contains(InputConfig))
		{
			auto HandleData = InputData.FindAndRemoveChecked(InputConfig);
			IC->RemoveBinds(HandleData.Data);
		}
	}
}

bool AGASXBaseCharacter::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

// Called every frame
void AGASXBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGASXBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InitializePlayerInput();
}

UAbilitySystemComponent* AGASXBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UGASXAbilitySystemComponent* AGASXBaseCharacter::GetGASXAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void AGASXBaseCharacter::InitializeMovementModeTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (AbilitySystemComponent.IsValid())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : GASXGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				AbilitySystemComponent->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		const auto CustomMovementModeTagMap = GetCustomMovementModeTagMap();
		for (const TPair<uint8, FGameplayTag>& TagMapping : CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				AbilitySystemComponent->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		SetMovementModeTag(GetCharacterMovement()->MovementMode, GetCharacterMovement()->CustomMovementMode, true);
	}
}

void AGASXBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(GetCharacterMovement()->MovementMode, GetCharacterMovement()->CustomMovementMode, true);
}

void AGASXBaseCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (AbilitySystemComponent.IsValid())
	{
		const FGameplayTag* MovementModeTag = nullptr;

		bool bFoundCustomTag = false;
		const auto CustomMovementModeTagMap = GetCustomMovementModeTagMap();
		if (MovementMode == MOVE_Custom && CustomMovementModeTagMap.Num() > 0)
		{
			MovementModeTag = CustomMovementModeTagMap.Find(CustomMovementMode);
			bFoundCustomTag = MovementModeTag != nullptr;
		}

		if (!bFoundCustomTag)
		{
			MovementModeTag = GASXGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			AbilitySystemComponent->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}
