// Copyright 2024 Toranosuke Ichikawa


#include "GameplayAbilities/GA_Passive_FindInteractableBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interfaces/GASXInteractable.h"
#include "GASXLibrary.h"
#include "AbilitySystemComponent.h"
#include "GASXMacroDefinitions.h"
#include "GASXTargetType.h"

namespace GASXConsoleVariables
{
	static bool DebugPassiveInteract = false;
	static FAutoConsoleVariableRef CVarDebugPassiveInteract(
		TEXT("gasx.interaction.debugpassiveinteract"),
		DebugPassiveInteract,
		TEXT("If true, debug passive interact ability."),
		ECVF_Cheat);
}

UGA_Passive_FindInteractableBase::UGA_Passive_FindInteractableBase()
	: Super()
	, TimerPeriod(0.1f)
	, bBlockInteractionIfTargetWasLost(false)
{
	ActivationPolicy = EGASXAbilityActivationPolicy::OnSpawn;	// Passive ability
}

void UGA_Passive_FindInteractableBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* OwnerInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, OwnerInfo, ActivationInfo, TriggerEventData);

	auto ASC = OwnerInfo->AbilitySystemComponent.Get();
	if (ASC)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(TimerHandle_LoopFindInteractable, this, &UGA_Passive_FindInteractableBase::TickFindInteractable, TimerPeriod, FTimerManagerTimerParameters{ .bLoop = true, .bMaxOncePerFrame = true, .FirstDelay = TimerPeriod });
			return;
		}
	}

	CancelAbility(Handle, OwnerInfo, ActivationInfo, true);
}

void UGA_Passive_FindInteractableBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle_LoopFindInteractable);
	}
}

void UGA_Passive_FindInteractableBase::TickFindInteractable()
{
	if (CurrentActorInfo && InteractionAbilityTag.IsValid())
	{
		auto ASC = CurrentActorInfo->AbilitySystemComponent;
		if (ASC.IsValid())
		{
			// Stops following operation if the owner ASC contains interaction abilities and any of them can be activated.  
			TArray<FGameplayAbilitySpec*> InteractionAbilities;
			ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(InteractionAbilityTag), InteractionAbilities, true);
			if (InteractionAbilities.Num() == 0)
			{
				bIsInteractionBlocked = true;
				CancelWaiting(true);
				LastData.Clear();
				return;
			}
			else
			{
				bIsInteractionBlocked = false;
			}
		}
	}

	if (bIsInteractionBlocked || !InteractionAbilityTag.IsValid())
	{
		return;
	}

	bool bFoundValidData = false;
	FGameplayAbilityTargetDataHandle FoundData;
	bFoundValidData = TryFindTargetInteractable(FoundData);
	if (!bFoundValidData) FoundData.Clear();	// set empty data if not found.

	if (bFoundValidData)
	{
		// Notifies target lost first, then notifies target found.

		// Check if it is new target and NOT previous target
		if (UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(FoundData, 0).GetComponent() != UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(LastData, 0).GetComponent())
		{
			if (IsTargetDataValid(LastData))
			{
				OnLostTarget();
			}

			OnFoundTarget(FoundData);
		}
	}
	else
	{
		if (IsTargetDataValid(LastData))
		{
			OnLostTarget();
		}
	}

	LastData = FoundData;
}

void UGA_Passive_FindInteractableBase::OnFoundTarget(const FGameplayAbilityTargetDataHandle& FoundData)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GASXConsoleVariables::DebugPassiveInteract)
	{
		UE_LOG(LogGASX, Log, TEXT("%s"), *FString("Found target: " + MakeTargetDataMessage(FoundData)));
	}
#endif

	if (!bIsInteracting)
	{
		CurrentTargetData = FoundData;

		// Notifies that target is found.
		FGameplayEventData Payload;
		Payload.EventTag = FoundInteractableTag;
		Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
		Payload.TargetData = CurrentTargetData;
		SendGameplayEvent(Payload.EventTag, Payload);
	}
}

void UGA_Passive_FindInteractableBase::OnLostTarget()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GASXConsoleVariables::DebugPassiveInteract)
	{
		UE_LOG(LogGASX, Log, TEXT("%s"), *FString("Lost target: " + MakeTargetDataMessage(LastData)));
	}
#endif

	CancelWaiting(bBlockInteractionIfTargetWasLost);
}

void UGA_Passive_FindInteractableBase::CancelWaiting(bool bCancelOngoingInteraction)
{
	if (bCancelOngoingInteraction)
	{
		EndInteraction(false);
	}

	// Notifies that target is lost.
	FGameplayEventData Payload;
	Payload.EventTag = LostInteractableTag;
	Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
	Payload.TargetData = CurrentTargetData;
	SendGameplayEvent(Payload.EventTag, Payload);

	CurrentTargetData.Clear();
}

void UGA_Passive_FindInteractableBase::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!bIsInteractionBlocked && !bIsInteracting && IsTargetDataValid(CurrentTargetData) && ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (GASXConsoleVariables::DebugPassiveInteract)
		{
			UE_LOG(LogGASX, Log, TEXT("%s"), *FString("Start Interaction: " + MakeTargetDataMessage(CurrentTargetData)));
		}
#endif

		bIsInteracting = true;
		InteractingTargetData = CurrentTargetData;

		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(InteractingTargetData, 0);
		float Duration = IGASXInteractable::Execute_GetInteractionDuration(HitResult.GetActor(), HitResult.GetComponent());

		// Notifies interaction start.
		FGameplayEventData Payload;
		Payload.EventTag = StartInteractionTag;
		Payload.Instigator = ActorInfo->AvatarActor.Get();
		Payload.TargetData = InteractingTargetData;
		Payload.EventMagnitude = Duration; // Pass interaction duration
		SendGameplayEvent(Payload.EventTag, Payload);

		// Triggers interaction ability
		Payload.EventTag = InteractionAbilityTag;
		SendGameplayEvent(Payload.EventTag, Payload);
	}
}

void UGA_Passive_FindInteractableBase::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndInteraction(true);
}

void UGA_Passive_FindInteractableBase::EndInteraction(bool bTryWaitAgain)
{
	if (CurrentActorInfo != NULL && CurrentActorInfo->AvatarActor != NULL)
	{
		bool bShouldEndInteraction = bIsInteracting && IsTargetDataValid(InteractingTargetData);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (GASXConsoleVariables::DebugPassiveInteract)
		{
			FString Prefix = bShouldEndInteraction ? "End Interaction: " : "Just Reset Target Data: ";
			UE_LOG(LogGASX, Log, TEXT("%s"), *FString(Prefix + MakeTargetDataMessage(InteractingTargetData)));
		}
#endif

		bIsInteracting = false;
		FGameplayAbilityTargetDataHandle EndTargetData = InteractingTargetData;
		InteractingTargetData.Clear();	// Reset

		if (bShouldEndInteraction)
		{
			// Notifies interaction end. This also stops ongoing interaction ability.
			FGameplayEventData Payload;
			Payload.EventTag = EndInteractionTag;
			Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
			Payload.TargetData = EndTargetData;
			SendGameplayEvent(Payload.EventTag, Payload);

			// This handles the case where interaction is finished by releasing the input but the current target is still the same. The owner might want to interact with the same target again.
			if (bTryWaitAgain
				&& !bIsInteractionBlocked
				&& IsTargetDataValid(CurrentTargetData)
				&& UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(CurrentTargetData, 0).GetActor() == UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(EndTargetData, 0).GetActor())
			{
				OnFoundTarget(EndTargetData);
			}
		}
	}
}

bool UGA_Passive_FindInteractableBase::TryFindTargetInteractable_Implementation(FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	float MinDistanceSpr = FLT_MAX;
	bool bIsDataValid = false;

	TArray<FHitResult> Targets;
	if (GetTargets(Targets))
	{
		for (auto T : Targets)
		{
			FGameplayAbilityTargetDataHandle Data;
			if (MakeValidTargetDataFromHitResult(T, Data))
			{
				if (T.GetComponent() != nullptr && CurrentActorInfo != nullptr && CurrentActorInfo->AvatarActor != nullptr)
				{
					float DistSqr = FVector::DistSquared(T.GetComponent()->GetComponentLocation(), CurrentActorInfo->AvatarActor->GetActorLocation());
					if (DistSqr < MinDistanceSpr)
					{
						bIsDataValid = true;
						MinDistanceSpr = DistSqr;
						OutTargetDataHandle = Data;
					}
				}
			}
		}
	}
	return bIsDataValid;
}

bool UGA_Passive_FindInteractableBase::GetTargets_Implementation(TArray<FHitResult>& OutHitResults)
{
	FGameplayEventData EventData;
	TArray<AActor*> TargetActors;
	const UGASXTargetType* TargetTypeCDO = TargetType.GetDefaultObject();
	TargetTypeCDO->GetTargets(*CurrentActorInfo, EventData, OutHitResults, TargetActors);
	return OutHitResults.Num() > 0;
}

bool UGA_Passive_FindInteractableBase::MakeValidTargetDataFromHitResult(const FHitResult& InHitResult, FGameplayAbilityTargetDataHandle& OutValidTargetData)
{
	FGameplayAbilityTargetDataHandle Result = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(InHitResult);
	if (IsTargetDataValid(Result))
	{
		OutValidTargetData = Result;
		return true;
	}
	return false;
}

bool UGA_Passive_FindInteractableBase::IsTargetDataValid_Implementation(const FGameplayAbilityTargetDataHandle& InTargetData)
{
	if (CurrentActorInfo != nullptr && CurrentActorInfo->AvatarActor != nullptr)
	{
		TArray<AActor*> IgnoreActors = TArray<AActor*>{ CurrentActorInfo->AvatarActor.Get() };
		return UGASXLibrary::IsTargetDataValidForInteraction(InTargetData, IgnoreActors, true);
	}
	return false;
}

FString UGA_Passive_FindInteractableBase::MakeTargetDataMessage(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (IsTargetDataValid(InTargetDataHandle))
	{
		return FString::Printf(TEXT("%s (%s)")
			, *(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(InTargetDataHandle, 0).GetComponent()->GetName())
			, *(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(InTargetDataHandle, 0).GetActor()->GetName())
		);
	}
	return "None";
}
