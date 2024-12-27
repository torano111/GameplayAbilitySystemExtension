// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GASXGameplayAbility.h"
#include "GA_Passive_FindInteractableBase.generated.h"

/**
 * This is a passive ability which runs forever to look for interactables around the owner and send it with gameplay events.
 * For actual interaction, see GA_PerformInteractBase.
 * The reason for splitting interaction into 2 gameplay abilities is so that the ability actually interacting can have blocked tags etc.
 *
 * This ability uses a timer to find a target actor which implements IGASXInteractable.
 * If a target is found, waits for input press and then send a gameplay event with StartInteractionTag to notify interaction start.
 * Its EventMagnitude is set to the interaction duration, and its target data includes a hit result with the interactable actor.
 * After starting interaction, this waits for input release and then send a gameplay event with EndInteractionTag to notify interaction end.
 * Also, this sends a gameplay event with the corresponding tag and target data when the target is found or lost so that other objects such as HUD can use it.
 *
 * If you want to stop the timer for a while, then use BlockInteractionTag.
 * 
 * ----- Usage -----
 * - make a child ability
 * - set StartInteractionTag. This tag must be the same as GA_PerformInteractBase's ability trigger tag
 * - set EndInteractionTag. This tag must be the same as GA_PerformInteractBase's EndInteractionTag
 * - (Optional) set FoundInteractableTag, LostInteractionTag, and BlockInteractionTag.
 * - set TargetType so that the subclass can find targets.
 */
UCLASS(Abstract)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGA_Passive_FindInteractableBase : public UGASXGameplayAbility
{
	GENERATED_BODY()

public:
	// Timer loop period
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability|FindInteractable")
	float TimerPeriod;

	// GameplayEvent tag to be sent when interaction starts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	FGameplayTag StartInteractionTag;

	// GameplayEvent tag to be sent when interaction ends
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	FGameplayTag EndInteractionTag;

	// GameplayEvent tag to be sent when an interaction target is found
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	FGameplayTag FoundInteractableTag;

	// GameplayEvent tag to be sent when the found target is lost
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	FGameplayTag LostInteractableTag;

	// When this tag is added to the owner, stop timer loop finding interactables, and end ongoing interaction immediately.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	FGameplayTag BlockInteractionTag;

	// Blocks ongoing interaction if we lost the target. 
	// e.g.block interaction when owner looks away from target while interacting.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	bool bBlockInteractionIfTargetWasLost;

	// Used to look for target candidates.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|FindInteractable")
	TSubclassOf<UGASXTargetType> TargetType;
	
protected:
	FDelegateHandle DelegateHandle;
	bool bRegisteredCallback = false;

	FTimerHandle TimerHandle_LoopFindInteractable;
	FGameplayAbilityTargetDataHandle LastData;

	FGameplayAbilityTargetDataHandle CurrentTargetData;
	bool bIsInteracting = false;
	bool bIsInteractionBlocked = false;

	FGameplayAbilityTargetDataHandle InteractingTargetData;
public:
	UGA_Passive_FindInteractableBase();

	// UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	// End of UGameplayAbility interface

protected:
	UFUNCTION()
	virtual void OnBlockInteractionTagAddedOrRemoved(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	virtual void TickFindInteractable();

	virtual void OnFoundTarget(const FGameplayAbilityTargetDataHandle& FoundData);
	virtual void OnLostTarget();

	virtual void CancelWaiting(bool bCancelOngoingInteraction);
	virtual void EndInteraction(bool bTryWaitAgain);


	// Tries to find a valid interactable target using GetTargets().
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|FindInteractable")
	bool TryFindTargetInteractable(FGameplayAbilityTargetDataHandle& OutTargetDataHandle);
	virtual bool TryFindTargetInteractable_Implementation(FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	// Returns true if any target is found. This might not check if targets are valid interactables.
	// By default, it uses TargetType to look for targets.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|FindInteractable")
	bool GetTargets(TArray<FHitResult>& OutHitResults);
	virtual bool GetTargets_Implementation(TArray<FHitResult>& OutHitResults);

	UFUNCTION(BlueprintCallable, Category = "Ability|FindInteractable")
	bool MakeValidTargetDataFromHitResult(const FHitResult& InHitResult, FGameplayAbilityTargetDataHandle& OutValidTargetData);

	// Checks if InTargetData is valid as a target.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|FindInteractable")
	bool IsTargetDataValid(const FGameplayAbilityTargetDataHandle& InTargetData);
	virtual bool IsTargetDataValid_Implementation(const FGameplayAbilityTargetDataHandle& InTargetData);

	UFUNCTION(BlueprintPure, Category = "Ability|FindInteractable")
	bool IsInteracting() const { return bIsInteracting; }

	UFUNCTION(BlueprintPure, Category = "Ability|FindInteractable")
	bool IsInteractionBlocked() const { return bIsInteractionBlocked; }

	FString MakeTargetDataMessage(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
};
