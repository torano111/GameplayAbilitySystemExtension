// Copyright 2024 Toranosuke Ichikawa

#include "GASXBaseCharacter.h"
#include "GASXPawnComponent.h"
#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"
#include "GASXInputComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASXGameplayTags.h"

// Sets default values
AGASXBaseCharacter::AGASXBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAutoInitGASOnPossessed(true)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GASXPawnComponent = ObjectInitializer.CreateDefaultSubobject<UGASXPawnComponent>(this, TEXT("GASXPawnComponent"));
}

// Called when the game starts or when spawned
void AGASXBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GASXPawnComponent)
	{
		if (GASXPawnComponent->IsInitialized())
		{
			auto NewAbilitySystemComponent = GASXPawnComponent->GetGASXAbilitySystemComponent();
			OnGASXPawnComponentInitialized(NewAbilitySystemComponent, NewAbilitySystemComponent ? NewAbilitySystemComponent->GetOwnerActor() : nullptr, NewAbilitySystemComponent ? NewAbilitySystemComponent->GetAvatarActor() : nullptr, Cast<AGASXPlayerState>(GetPlayerState()));
		}
		else
		{
			GASXPawnComponent->OnInitialized.AddDynamic(this, &AGASXBaseCharacter::OnGASXPawnComponentInitialized);
		}
	}
}

void AGASXBaseCharacter::OnGASXPawnComponentInitialized(UGASXAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor, AActor* InAvatarActor, AGASXPlayerState* InPlayerState)
{	
	AbilitySystemComponent = InAbilitySystemComponent;

	InitializeMovementModeTags();
}

void AGASXBaseCharacter::PossessedBy(AController* NewController)
{
	// Initialize GASXPawnComponent before ReceivePossessed (BP event Possessed) if needed
	if (bAutoInitGASOnPossessed)
	{
		AGASXPlayerState* PS = NewController ? Cast<AGASXPlayerState>(NewController->PlayerState) : nullptr;
		if (ensureMsgf(PS && GASXPawnComponent, TEXT("Failed to initialize GASXPawnComponent on possessed: PlayerState=%s, GASXPawnComponent=%s"), *FString(PS ? PS->GetName() : "NULL"), *FString(GASXPawnComponent ? GASXPawnComponent->GetName() : "NULL")))
		{
			GASXPawnComponent->Initialize(PS->GetGASXAbilitySystemComponent(), PS, PS);
		}
	}

	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
void AGASXBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto IC = Cast<UGASXInputComponent>(PlayerInputComponent);
	if (ensureMsgf(IC && GASXPawnComponent, TEXT("Failed to initialize player input: GASXInputComponent=%s, GASXPawnComponent=%s"), *FString(IC ? IC->GetName() : "NULL"), *FString(GASXPawnComponent ? GASXPawnComponent->GetName() : "NULL")))
	{
		GASXPawnComponent->InitializePlayerInput(IC);
	}
	
}

UAbilitySystemComponent* AGASXBaseCharacter::GetAbilitySystemComponent() const
{
	return GetGASXAbilitySystemComponent();
}

UGASXAbilitySystemComponent* AGASXBaseCharacter::GetGASXAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

UGASXPawnComponent* AGASXBaseCharacter::GetGASXPawnComponent() const
{
	return GASXPawnComponent;
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
