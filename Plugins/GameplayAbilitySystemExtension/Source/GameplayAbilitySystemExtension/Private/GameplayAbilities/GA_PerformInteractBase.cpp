// Copyright 2024 Toranosuke Ichikawa

#include "GameplayAbilities/GA_PerformInteractBase.h"
#include "Interfaces/GASXInteractable.h"
#include "GASXLibrary.h"
#include "AbilitySystemComponent.h"
#include "GASXMacroDefinitions.h"
#include "Components/PrimitiveComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

namespace GASXConsoleVariables
{
	static bool DebugInteract = false;
	static FAutoConsoleVariableRef CVarDebugInteract(
		TEXT("gasx.interaction.debuginteract"),
		DebugInteract,
		TEXT("If true, debug main interact ability."),
		ECVF_Default);
}

UGA_PerformInteractBase::UGA_PerformInteractBase()
	: Super()
{
	ActivationPolicy = EGASXAbilityActivationPolicy::OnInputTriggered;
}

void UGA_PerformInteractBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	FString FailReason = "";
	if (TriggerEventData != nullptr)
	{
		if (EndInteractionTag.IsValid() && OwnerInfo != nullptr && OwnerInfo->AbilitySystemComponent != nullptr)
		{
			OnEndInteractionHandle = OwnerInfo->AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(EndInteractionTag).AddUObject(this, &UGA_PerformInteractBase::OnEndInteractionEvent);
		}

		if (IsTargetDataValid(TriggerEventData->TargetData))
		{
			CurrentTargetData = TriggerEventData->TargetData;

			AActor* InteractableActor = nullptr;
			UPrimitiveComponent* InteractableComponent = nullptr;
			if (GetCurrentTargetInfo(InteractableActor, InteractableComponent))
			{
				bIsInteracting = true;

				IGASXInteractable::Execute_PreInteract(InteractableActor, OwnerInfo->AvatarActor.Get(), InteractableComponent);

				bShouldActivateAbilityOnPreInteract = IGASXInteractable::Execute_ShouldActivateAbilityOnPreInteract(InteractableActor);
				if (bShouldActivateAbilityOnPreInteract)
				{
					ExecuteInteractableAbility(InteractablePreInteractTag);
				}

				float InteractionDuration = IGASXInteractable::Execute_GetInteractionDuration(InteractableActor, InteractableComponent);
				if (InteractionDuration <= 0.f)
				{
					PerformMainInteraction();
					return;
				}
				else if (UWorld* World = GetWorld())
				{
					FTimerHandle TimerHandle_Delay;
					World->GetTimerManager().SetTimer(TimerHandle_Delay, this, &UGA_PerformInteractBase::PerformMainInteraction, InteractionDuration, false);
					return;
				}
				FailReason = "PerformMainInteraction is not executed for some reason";
			}
		}
		else
		{
			FailReason = "Target data is NOT valid";
		}
	}
	else
	{
		FailReason = "TriggerEventData is null";
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GASXConsoleVariables::DebugInteract)
	{
		UE_LOG(LogGASX, Error, TEXT("Failed to activate UGA_PerformInteractBase properly because %s."), *FailReason);
	}
#endif
	CancelAbility(Handle, OwnerInfo, ActivationInfo, true);
}

void UGA_PerformInteractBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ActorInfo != nullptr && ActorInfo->AbilitySystemComponent != nullptr)
		{
			ActorInfo->AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(EndInteractionTag).Remove(OnEndInteractionHandle);
		}

		if (bWasCancelled) CancelInteraction();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_PerformInteractBase::PerformMainInteraction()
{
	if (bIsInteracting)
	{
		AActor* InteractableActor = nullptr;
		UPrimitiveComponent* InteractableComponent = nullptr;
		if (GetCurrentTargetInfo(InteractableActor, InteractableComponent))
		{
			IGASXInteractable::Execute_Interact(InteractableActor, CurrentActorInfo->AvatarActor.Get(), InteractableComponent);

			if (bShouldActivateAbilityOnPreInteract)
			{
				FGameplayEventData Payload;
				Payload.EventTag = InteractableInteractTag;
				Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
				Payload.TargetData = CurrentTargetData;
				SendGameplayEvent(Payload.EventTag, Payload);
			}
			else
			{
				ExecuteInteractableAbility(InteractableInteractTag);
			}

			FGameplayEventData Payload;
			Payload.EventTag = ExecuteInteractionTag;
			Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
			Payload.TargetData = CurrentTargetData;
			SendGameplayEvent(Payload.EventTag, Payload);

			bIsInteracting = false;
			InteractableAbilityHandle = FGameplayAbilitySpecHandle();	// invalidate
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
}

void UGA_PerformInteractBase::OnEndInteractionEvent(const FGameplayEventData* Payload)
{
	// Cancels ongoing interaction
	if (bIsInteracting && IsActive())
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (GASXConsoleVariables::DebugInteract)
		{
			UE_LOG(LogGASX, Log, TEXT("UGA_PerformInteractBase was canceled."));
		}
#endif
		bIsInteracting = false;
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

bool UGA_PerformInteractBase::GetCurrentTargetInfo(AActor*& InteractableActor, UPrimitiveComponent*& InteractableComponent)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(CurrentTargetData, 0))
	{
		auto HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(CurrentTargetData, 0);
		if (IsValid(HitResult.GetActor()) && IsValid(HitResult.GetComponent()))
		{
			InteractableActor = HitResult.GetActor();
			InteractableComponent = HitResult.GetComponent();
			return true;
		}
	}
	return false;
}

void UGA_PerformInteractBase::ExecuteInteractableAbility(FGameplayTag EventTag)
{
	AActor* InteractableActor = nullptr;
	UPrimitiveComponent* InteractableComponent = nullptr;
	if (GetCurrentTargetInfo(InteractableActor, InteractableComponent))
	{
		TSubclassOf<UGameplayAbility> AbilityClass = IGASXInteractable::Execute_GetInteractionAbility(InteractableActor);
		if (IsValid(AbilityClass))
		{
			FGameplayEventData Payload;
			Payload.EventTag = EventTag;
			Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
			Payload.TargetData = CurrentTargetData;
			Payload.Target = InteractableActor;
			InteractableAbilityHandle = UGASXLibrary::GiveAbilityAndActivateOnceWithGameplayEvent(CurrentActorInfo->AbilitySystemComponent.Get(), AbilityClass, Payload, 0, -1);
		}
	}
}

void UGA_PerformInteractBase::CancelInteraction()
{
	// If we can get an ability instance from the ability spec handle, then cancel it.
	if (CurrentActorInfo != nullptr && CurrentActorInfo->AbilitySystemComponent != nullptr)
	{
		FGameplayAbilitySpec* AbilitySpec = CurrentActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(InteractableAbilityHandle);
		if (AbilitySpec)
		{
			// try to get the ability instance
			UGameplayAbility* AbilityInstance = AbilitySpec->GetPrimaryInstance();
			// default to the CDO if we can't
			if (!AbilityInstance) AbilityInstance = AbilitySpec->Ability;

			if (AbilityInstance) AbilityInstance->CancelAbility(AbilityInstance->GetCurrentAbilitySpecHandle(), AbilityInstance->GetCurrentActorInfo(), AbilityInstance->GetCurrentActivationInfo(), true);
		}
	}

	AActor* InteractableActor = nullptr;
	UPrimitiveComponent* InteractableComponent = nullptr;
	if (GetCurrentTargetInfo(InteractableActor, InteractableComponent))
	{
		IGASXInteractable::Execute_CancelInteraction(InteractableActor, CurrentActorInfo->AvatarActor.Get(), InteractableComponent);
	}
}

bool UGA_PerformInteractBase::IsTargetDataValid_Implementation(const FGameplayAbilityTargetDataHandle& InTargetData)
{
	if (CurrentActorInfo != nullptr && CurrentActorInfo->AvatarActor != nullptr)
	{
		TArray<AActor*> IgnoreActors = TArray<AActor*>{ CurrentActorInfo->AvatarActor.Get() };
		return UGASXLibrary::IsTargetDataValidForInteraction(InTargetData, IgnoreActors, true);
	}
	return false;
}
