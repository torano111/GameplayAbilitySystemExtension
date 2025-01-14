// Copyright 2024 Toranosuke Ichikawa

#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"
#include "Experience/GASXExperienceManagerComponent.h"
#include "GASXMacroDefinitions.h"
#include "GASXGameMode.h"
#include "DataAssets/GASXPawnData.h"
#include "DataAssets/GASXAbilitySet.h"
#include "GASXLibrary.h"

AGASXPlayerState::AGASXPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create ability system component. 
	// This project doesn't support multiplayer for now.
	AbilitySystemComponent = CreateDefaultSubobject<UGASXAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
}

void AGASXPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		UGASXExperienceManagerComponent* ExperienceComponent = UGASXLibrary::GetExperienceManagerComponent(World);
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnGASXExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

UAbilitySystemComponent* AGASXPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGASXAbilitySystemComponent* AGASXPlayerState::GetGASXAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const UGASXPawnData* AGASXPlayerState::GetPawnData() const
{
	return PawnData.Get();
}

void AGASXPlayerState::SetPawnData(const UGASXPawnData* InPawnData)
{
	check(InPawnData);
	if (PawnData)
	{
		UE_LOG(LogGASX, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	//MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	//for (const UGASXAbilitySet* AbilitySet : PawnData->AbilitySets)
	//{
	//	if (AbilitySet)
	//	{
	//		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	//	}
	//}

	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_LyraAbilityReady);
}

void AGASXPlayerState::OnExperienceLoaded(const UGASXExperienceDefinition* CurrentExperience)
{
	if (AGASXGameMode* GASXGameMode = GetWorld()->GetAuthGameMode<AGASXGameMode>())
	{
		if (const UGASXPawnData* NewPawnData = GASXGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogGASX, Error, TEXT("AGASXPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}
