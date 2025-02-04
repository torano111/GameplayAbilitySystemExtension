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
#include "GameplayEffects/AttributeSetInitializer.h"
#include "DataAssets/GASXPawnData.h"
#include "GASXBaseCharacter.h"
#include "AIController.h"

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
///// Attribute Set Initializer

FActiveGameplayEffectHandle UGASXLibrary::ApplyAttributeSetInitializer(UAbilitySystemComponent* ASC, FAttributeSetInitializer AttributeSetInitializer, float Level, FGameplayEffectContextHandle EffectContext)
{
	if (ASC != nullptr && AttributeSetInitializer.IsValid())
	{
		if (!EffectContext.IsValid())
		{
			EffectContext = ASC->MakeEffectContext();
		}

		return AttributeSetInitializer.Apply(ASC, Level, EffectContext);
	}

	return FActiveGameplayEffectHandle();
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
				UGameplayStatics::OpenLevel(WorldContextObject, *TravelURL);
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

APawn* UGASXLibrary::SpawnBotWithPawnData(const UObject* WorldContextObject, TSubclassOf<class AAIController> BotControllerClass, UGASXPawnData* BotPawnData, const FTransform& SpawnTransform, AActor* Owner, APawn* Instigator, ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride, ESpawnActorScaleMethod TransformScaleMethod)
{
	APawn* SpawnedPawn = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (BotPawnData != nullptr && BotControllerClass)
		{
			// Spawn AI Controller
			FActorSpawnParameters ControllerSpawnInfo;
			ControllerSpawnInfo.Owner = Owner;
			ControllerSpawnInfo.Instigator = Instigator;
			ControllerSpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ControllerSpawnInfo.ObjectFlags |= RF_Transient;
			AAIController* NewController = World->SpawnActor<AAIController>(BotControllerClass, FVector::ZeroVector, FRotator::ZeroRotator, ControllerSpawnInfo);

			if (NewController)
			{
				// Spawn bot Pawn
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Owner = Owner;
				SpawnInfo.Instigator = Instigator;
				SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
				SpawnInfo.bDeferConstruction = true;
				if (auto PawnClass = BotPawnData->PawnClass)
				{
					SpawnedPawn = World->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo);
					if (SpawnedPawn)
					{
						if (AGASXBaseCharacter* GASXBaseCharacter = Cast<AGASXBaseCharacter>(SpawnedPawn))
						{
							GASXBaseCharacter->SetPawnData(BotPawnData);
						}
						else
						{
							UE_LOG(LogGASX, Error, TEXT("UGASXLibrary::SpawnBotWithPawnData: failed to set pawn data to bot because the spawned pawn(%s) is not a subclass of AGASXBaseCharacter."), *GetNameSafe(SpawnedPawn));
						}

						SpawnedPawn->FinishSpawning(SpawnTransform);

						if (IsValid(SpawnedPawn))
						{
							NewController->SetPawn(SpawnedPawn);
							NewController->Possess(NewController->GetPawn());
						}
					}
					else
					{
						UE_LOG(LogGASX, Error, TEXT("UGASXLibrary::SpawnBotWithPawnData: failed to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
					}
				}
				else
				{
					UE_LOG(LogGASX, Error, TEXT("UGASXLibrary::SpawnBotWithPawnData: failed to spawn Pawn due to NULL pawn class."));
				}
			}
			else
			{
				UE_LOG(LogGASX, Error, TEXT("UGASXLibrary::SpawnBotWithPawnData: failed to spawn AIController due to NULL pawn class."));
			}
		}
	}

	return SpawnedPawn;
}

////////////////////
///// Misc.

void UGASXLibrary::ClearTargetData(UPARAM(ref)FGameplayAbilityTargetDataHandle& InOutTargetData)
{
	InOutTargetData.Clear();
}
