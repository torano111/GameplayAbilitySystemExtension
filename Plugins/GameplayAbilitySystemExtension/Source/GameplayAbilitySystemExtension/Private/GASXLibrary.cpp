// Copyright 2024 Toranosuke Ichikawa

#include "GASXLibrary.h"
#include "GASXMacroDefinitions.h"
#include "GASXDataTypes.h"
#include "GASXTargetType.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/GASXInteractable.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Experience/GASXUserFacingExperienceDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Experience/GASXExperienceManagerComponent.h"
#include "GameFramework/GameModeBase.h"

////////////////////
///// UAbilitySystemComponent

FGameplayAbilitySpecHandle UGASXLibrary::GiveAbilityAndActivateOnceWithGameplayEvent(UAbilitySystemComponent* TargetASC, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData Payload, int32 Level, int32 InputID)
{
	if (TargetASC)
	{
		// build and validate the ability spec
		FGameplayAbilitySpec AbilitySpec = TargetASC->BuildAbilitySpecFromClass(AbilityClass, Level, InputID);

		// validate the class
		if (!IsValid(AbilitySpec.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("K2_GiveAbilityAndActivateOnce() called with an invalid Ability Class."));

			return FGameplayAbilitySpecHandle();
		}

		return TargetASC->GiveAbilityAndActivateOnce(AbilitySpec, &Payload);
	}

	UE_LOG(LogTemp, Error, TEXT("GiveAbilityAndActivateOnceWithGameplayEvent() called, but TargetASC is nullptr."));
	return FGameplayAbilitySpecHandle();
}

////////////////////
///// Target Type

void UGASXLibrary::GetTargetTypeTargets(TSubclassOf<class UGASXTargetType> TargetType, FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors)
{
	const UGASXTargetType* TargetTypeCDO = TargetType.GetDefaultObject();
	TargetTypeCDO->GetTargets(ActorInfo, EventData, OutHitResults, OutActors);
}

////////////////////
///// Interaction

bool UGASXLibrary::IsTargetDataValidForInteraction(const FGameplayAbilityTargetDataHandle& InTargetData, TArray<AActor*> IgnoreActors, bool bChecksAvailability)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(InTargetData, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(InTargetData, 0);
		AActor* HitActor = HitResult.GetActor();

		return HitActor != nullptr
			&& !IgnoreActors.Contains(HitActor)
			&& HitResult.GetComponent() != nullptr
			&& HitActor->GetClass()->ImplementsInterface(UGASXInteractable::StaticClass())
			&& (!bChecksAvailability || IGASXInteractable::Execute_IsAvailableForInteraction(HitActor, HitResult.GetComponent()));
	}
	return false;
}

////////////////////
///// Effect Container

bool UGASXLibrary::DoesEffectContainerSpecHaveEffects(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidEffects();
}

bool UGASXLibrary::DoesEffectContainerSpecHaveTargets(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	return ContainerSpec.HasValidTargets();
}

FGASXGameplayEffectContainerSpec UGASXLibrary::AddTargetsToEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	FGASXGameplayEffectContainerSpec NewSpec = ContainerSpec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> UGASXLibrary::ApplyExternalEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
			{
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
			}
		}
	}
	return AllEffects;
}

////////////////////
///// Experience

void UGASXLibrary::TravelToExperience(const UObject* WorldContextObject, UGASXUserFacingExperienceDefinition* UserFacingExperienceDefinition)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			if (IsValid(UserFacingExperienceDefinition))
			{
				FString TravelURL = UserFacingExperienceDefinition->ConstructTravelURL();
				UE_LOG(LogGASXExperience, Log, TEXT("UGASXLibrary::TravelToExperience: URL=%s"), *TravelURL);
				World->ServerTravel(TravelURL);
			}
		}
	}
}

UGASXExperienceManagerComponent* UGASXLibrary::GetExperienceManagerComponent(const UObject* WorldContextObject)
{
	UGASXExperienceManagerComponent* Component = nullptr;
	if (auto GameMode = UGameplayStatics::GetGameMode(WorldContextObject))
	{
		Component = GameMode->FindComponentByClass<UGASXExperienceManagerComponent>();
	}
	return Component;
}

////////////////////
///// Misc.

void UGASXLibrary::ClearTargetData(UPARAM(ref)FGameplayAbilityTargetDataHandle& InOutTargetData)
{
	InOutTargetData.Clear();
}
