// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GASXGameplayAbility.h"
#include "GA_PerformInteractBase.generated.h"

/**
 * This is an ability which receives a target interactable and interact with it. 
 * For finding the target, see GA_Passive_FindInteractableBase.
 * The reason for splitting interaction into 2 gameplay abilities is so that this ability can have blocked tags etc.
 *
 * This ability should be triggered with a gameplay event with target data including a single hit result with an actor which implements IGASXInteractable.
 * If its interaction duration > 0, then this waits for that duration. 
 * If this receives gameplay event with EndInteractionTag during waiting, then interaction will be canceled.
 * When interaction is executed, a gameplay event with ExecuteInteractionTag will be sent. This sends the same target data.
 *
 * ----- Usage -----
 * - make a child ability
 * - set EndInteractionTag. This tag must be the same as GA_Passive_FindInteractableBase's EndInteractionTag
 * - (Optional) set ExecuteInteractionTag.
 * - add an ability trigger
 * - its TriggerTag must be the same as AbilityTag and GA_Passive_FindInteractableBase's InteractionAbilityTag
 * - its TriggerSouce must be GameplayEvent
 * - grant the ability to ASC. Don't activate this manually because this will be activated by GA_Passive_FindInteractableBase using gameplay event with StartInteractionTag.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGA_PerformInteractBase : public UGASXGameplayAbility
{
	GENERATED_BODY()
	
public:
	// GameplayEvent tag to cancel/end this ability if the corresponding game event is received before interaction finishes.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|PerformInteract")
	FGameplayTag EndInteractionTag;

	// GameplayEvent tag to be sent when interaction is actually executed.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|PerformInteract")
	FGameplayTag ExecuteInteractionTag;

	// GameplayEvent tag to be sent together when the interactable ability is triggered on pre-interaction.
	// The event won't be sent if the given interactable doesn't provide an ability for pre - interaction.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|PerformInteract")
	FGameplayTag InteractablePreInteractTag;

	// GameplayEvent tag to be sent together when the interactable ability is triggered on interaction.
	// The event won't be sent if the given interactable doesn't provide the ability.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|PerformInteract")
	FGameplayTag InteractableInteractTag;

protected:
	FGameplayAbilityTargetDataHandle CurrentTargetData;
	FGameplayAbilitySpecHandle InteractableAbilityHandle;
	FDelegateHandle OnEndInteractionHandle;
	bool bIsInteracting = false;
	bool bShouldActivateAbilityOnPreInteract = false;

public:
	UGA_PerformInteractBase();

	// UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// End of UGameplayAbility interface

protected:
	void PerformMainInteraction();

	void OnEndInteractionEvent(const FGameplayEventData* Payload);

	UFUNCTION(BlueprintCallable, Category = "Ability|PerformInteract")
	bool GetCurrentTargetInfo(AActor*& InteractableActor, class UPrimitiveComponent*& InteractableComponent);

	void ExecuteInteractableAbility(FGameplayTag EventTag);

	UFUNCTION(BlueprintCallable, Category = "Ability|PerformInteract")
	void CancelInteraction();

	// Checks if InTargetData is valid as a target.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability|PerformInteract")
	bool IsTargetDataValid(const FGameplayAbilityTargetDataHandle& InTargetData);
	virtual bool IsTargetDataValid_Implementation(const FGameplayAbilityTargetDataHandle& InTargetData);
};
