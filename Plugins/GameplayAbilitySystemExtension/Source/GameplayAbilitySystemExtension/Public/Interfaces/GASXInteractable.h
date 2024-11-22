// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GASXInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGASXInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base interface for an object which can be interacted with.
 * Intended for 2 interaction types: holding an interaction key for continuous interaction and interaction triggering immediately upon key press.
 * There is only basic functionalities, so you can inherit from and extend this interface as needed. 
 */
class GAMEPLAYABILITYSYSTEMEXTENSION_API IGASXInteractable
{
	GENERATED_BODY()

public:
	/**
	* Is this object available for player interaction at right now?
	*
	* @param InteractableComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GASXInteractable")
	bool IsAvailableForInteraction(UPrimitiveComponent* InteractableComponent) const;

	/**
	* Interact with this Actor. This will call before starting the Interact Duration timer. This might do things, apply
	* (predictively or not) GameplayEffects, trigger (predictively or not) GameplayAbilities, etc.
	*
	* You can use this function to grant abilities that will be predictively activated on Interact() to hide the
	* AbilitySpec replication time.
	*
	* If you want to do something predictively, you can get the ASC from the InteractingActor and use its
	* ScopedPredictionKey.
	*
	* Player revives use PreInteract() to trigger a ability that plays an animation that lasts the same duration as
	* the Interact Duration. If this ability finishes, it will revive the player in Interact().
	*
	* @param Interactor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
	* @param InteractableComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GASXInteractable")
	void PreInteract(AActor* Interactor, UPrimitiveComponent* InteractableComponent);

	/**
	* Interact with this Actor. This will call after the Interact Duration timer completes. This might do things, apply
	* (predictively or not) GameplayEffects, trigger (predictively or not) GameplayAbilities, etc.
	*
	* If you want to do something predictively, you can get the ASC from the InteractingActor and use its
	* ScopedPredictionKey.
	*
	* If you need to trigger a GameplayAbility predictively, the player's ASC needs to have been granted the ability
	* ahead of time. If you don't want to grant every possible predictive ability at game start, you can hide the time
	* needed to replicate the AbilitySpec inside the time needed to interact by granted it in PreInteract().
	*
	* @param Interactor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
	* @param InteractableComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void Interact(AActor* Interactor, UPrimitiveComponent* InteractableComponent);

	/**
	* How long does the player need to hold down the interact button to interact with this?
	* If 0, Interaction will be executed without holding the button (just pressing the button).
	* @param InteractableComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	float GetInteractionDuration(UPrimitiveComponent* InteractableComponent) const;

	/**
	* Cancel an ongoing interaction, typically anything happening in PreInteract() while waiting on the Interact Duration
	* Timer. This will be called if the player releases input early.
	*
	* @param Interactor The Actor interacting with this Actor. It will be the AvatarActor from a GameplayAbility.
	* @param InteractableComponent UPrimitiveComponent in case an Actor has many separate interactable areas.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void CancelInteraction(AActor* Interactor, UPrimitiveComponent* InteractableComponent);
};
