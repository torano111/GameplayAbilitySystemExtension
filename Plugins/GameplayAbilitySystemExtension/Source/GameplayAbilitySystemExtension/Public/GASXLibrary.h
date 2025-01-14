// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GASXLibrary.generated.h"

struct FGASXGameplayEffectContainerSpec;
class UGASXUserFacingExperienceDefinition;
class UGASXExperienceManagerComponent;

/**
 * 
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	////////////////////
	///// UAbilitySystemComponent

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Gameplay Abilities")
	static FGameplayAbilitySpecHandle GiveAbilityAndActivateOnceWithGameplayEvent(UAbilitySystemComponent* TargetASC, TSubclassOf<UGameplayAbility> AbilityClass, FGameplayEventData Payload, int32 Level = 0, int32 InputID = -1);

	////////////////////
	///// Target Type

	UFUNCTION(BlueprintCallable, Category = Ability)
	static void GetTargetTypeTargets(TSubclassOf<class UGASXTargetType> TargetType, FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors);
	
	////////////////////
	///// Interaction

	UFUNCTION(BlueprintCallable, Category = Ability)
	static bool IsTargetDataValidForInteraction(const FGameplayAbilityTargetDataHandle& InTargetData, TArray<AActor*> IgnoreActors, bool bChecksAvailability = true);

	////////////////////
	///// Effect Container

	/** Checks if spec has any effects */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveEffects(const FGASXGameplayEffectContainerSpec& ContainerSpec);

	/** Checks if spec has any targets */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveTargets(const FGASXGameplayEffectContainerSpec& ContainerSpec);

	/** Adds targets to a copy of the passed in effect container spec and returns it */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FGASXGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Applies container spec that was made from an ability */
	UFUNCTION(BlueprintCallable, Category = Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FGASXGameplayEffectContainerSpec& ContainerSpec);
	
	////////////////////
	///// Experience

	UFUNCTION(BlueprintCallable, Category = "Experience", meta = (WorldContext = "WorldContextObject"))
	static void TravelToExperience(const UObject* WorldContextObject, UGASXUserFacingExperienceDefinition* UserFacingExperienceDefinition);

	UFUNCTION(BlueprintPure, Category = "Experience", meta = (WorldContext = "WorldContextObject"))
	static UGASXExperienceManagerComponent* GetExperienceManagerComponent(const UObject* WorldContextObject);

	////////////////////
	///// Misc.

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static void ClearTargetData(UPARAM(ref) FGameplayAbilityTargetDataHandle& InOutTargetData);
};
