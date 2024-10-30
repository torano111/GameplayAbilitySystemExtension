// Copyright 2024 Toranosuke Ichikawa

#include "GASXTargetType.h"
#include "GASXBaseCharacter.h"
#include "GASXGameplayAbility.h"

UObject* UGASXTargetType::GetWorldContextObjectFromActorInfo(FGameplayAbilityActorInfo ActorInfo) const
{
	check(ActorInfo.OwnerActor.IsValid());
	return ActorInfo.OwnerActor->GetWorld();
}

void UGASXTargetType::GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void UGASXTargetType_UseOwner::GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (ActorInfo.OwnerActor.IsValid()) OutActors.Add(ActorInfo.OwnerActor.Get());
}

void UGASXTargetType_UseAvatar::GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (ActorInfo.AvatarActor.IsValid()) OutActors.Add(ActorInfo.AvatarActor.Get());
}

void UGASXTargetType_UseEventData::GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target.Get()));
	}
}

UObject* UGASXTargetType_TraceBase::GetSceneObject(FGameplayAbilityActorInfo ActorInfo) const
{
	switch (SceneObjectType)
	{
	case ESceneObjectType::SOT_OwnerActor:
		if (ActorInfo.OwnerActor.IsValid()) return ActorInfo.OwnerActor.Get();
		break;
	case ESceneObjectType::SOT_AvatarActor:
		if (ActorInfo.AvatarActor.IsValid()) return ActorInfo.AvatarActor.Get();
		break;
	case ESceneObjectType::SOT_SkeletalMeshComponent:
		if (ActorInfo.SkeletalMeshComponent.IsValid()) return ActorInfo.SkeletalMeshComponent.Get();
		break;
	case ESceneObjectType::SOT_None:
	default:
		break;
	}
	return nullptr;
}

FTransform UGASXTargetType_TraceBase::GetSceneObjectTransform(FGameplayAbilityActorInfo ActorInfo) const
{
	switch (SceneObjectType)
	{
	case ESceneObjectType::SOT_OwnerActor:
		if (ActorInfo.OwnerActor.IsValid()) return ActorInfo.OwnerActor->GetActorTransform();
		break;
	case ESceneObjectType::SOT_AvatarActor:
		if (ActorInfo.AvatarActor.IsValid()) return ActorInfo.AvatarActor->GetActorTransform();
		break;
	case ESceneObjectType::SOT_SkeletalMeshComponent:
		if (ActorInfo.SkeletalMeshComponent.IsValid()) return ActorInfo.SkeletalMeshComponent->GetComponentTransform();
		break;
	case ESceneObjectType::SOT_None:
	default:
		break;
	}
	return FTransform();
}

void UGASXTargetType_TraceBase::GetTraceStartAndEnd_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, FVector& OutStart, FVector& OutEnd) const
{
	return;
}

TArray<AActor*> UGASXTargetType_TraceBase::GetActorsToIgnore_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData) const
{
	TArray<AActor*> Actors;
	if (ActorInfo.AvatarActor.IsValid()) Actors.Add(ActorInfo.AvatarActor.Get());
	return Actors;
}

void UGASXTargetType_TraceBase::GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	FVector Start, End;
	GetTraceStartAndEnd(ActorInfo, EventData, Start, End);

	TArray<AActor*> ActorsToIgnore = GetActorsToIgnore(ActorInfo, EventData);

	bool bHit = false;
	switch (TraceType)
	{
	case EGASXTraceTargetType::TTT_ByChannel:
		bHit = UKismetSystemLibrary::SphereTraceMulti(
			GetWorldContextObjectFromActorInfo(ActorInfo),
			Start,
			End,
			SphereRadius,
			TraceChannel,
			bTraceComplex,
			ActorsToIgnore,
			DrawDebugType,
			OutHitResults,
			true,
			DebugTraceColor,
			DebugTraceHitColor,
			DebugDrawTime
		);
		break;
	case EGASXTraceTargetType::TTT_ByProfile:
		bHit = UKismetSystemLibrary::SphereTraceMultiByProfile(
			GetWorldContextObjectFromActorInfo(ActorInfo),
			Start,
			End,
			SphereRadius,
			ProfileName,
			bTraceComplex,
			ActorsToIgnore,
			DrawDebugType,
			OutHitResults,
			true,
			DebugTraceColor,
			DebugTraceHitColor,
			DebugDrawTime
		);
		break;
	case EGASXTraceTargetType::TTT_ByObjectTypes:
		bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
			GetWorldContextObjectFromActorInfo(ActorInfo),
			Start,
			End,
			SphereRadius,
			ObjectTypes,
			bTraceComplex,
			ActorsToIgnore,
			DrawDebugType,
			OutHitResults,
			true,
			DebugTraceColor,
			DebugTraceHitColor,
			DebugDrawTime
		);
		break;
	default:
		break;
	}

	if (bHit && bHitActorsAsTargets)
	{
		for (const auto& Hit : OutHitResults)
		{
			// no duplicates
			OutActors.AddUnique(Hit.GetActor());
		}
		OutHitResults.Empty();
	}
}

#if WITH_EDITOR
TArray<FName> UGASXTargetType_TraceBase::GetCollisionProfileNames()
{
	TArray<TSharedPtr<FName>> SharedNames;
	UCollisionProfile::GetProfileNames(SharedNames);

	TArray<FName> Names;
	Names.Reserve(SharedNames.Num());
	for (const TSharedPtr<FName>& SharedName : SharedNames)
	{
		if (const FName* Name = SharedName.Get())
		{
			Names.Add(*Name);
		}
	}

	return Names;
}
#endif

void UGASXTargetType_TraceFromAvatarActor::GetTraceStartAndEnd_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, FVector& OutStart, FVector& OutEnd) const
{
	FTransform BaseTransform = GetSceneObjectTransform(ActorInfo);
	FVector BaseForward = BaseTransform.GetUnitAxis(EAxis::X);

	OutStart = BaseTransform.TransformPosition(OffsetFromActor);
	OutEnd = BaseTransform.GetLocation() + BaseForward * TraceLength;
}
