// Copyright 2024 Toranosuke Ichikawa
// This code is from ActionRPG sample

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GASXDataTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GASXTargetType.generated.h"

class AGASXBaseCharacter;
class AActor;
struct FGameplayEventData;

UENUM(BlueprintType)
enum class ESceneObjectType : uint8
{
	SOT_None					UMETA(DisplayName = "None"),
	SOT_OwnerActor				UMETA(DisplayName = "OwnerActor"),
	SOT_AvatarActor				UMETA(DisplayName = "AvatarActor"),
	SOT_SkeletalMeshComponent	UMETA(DisplayName = "SkeletalMeshComponent")
};

UENUM(BlueprintType)
enum class EGASXTraceTargetType : uint8
{
	TTT_ByChannel		UMETA(DisplayName = "ByChannel"),
	TTT_ByProfile		UMETA(DisplayName = "ByProfile"),
	TTT_ByObjectTypes	UMETA(DisplayName = "ByObjectTypes")
};

UENUM(BlueprintType)
enum class EGASXTraceShapeType : uint8
{
	TST_LineTrace		UMETA(DisplayName = "LineTrace"),
	TST_SphereTrace		UMETA(DisplayName = "SphereTrace"),
	TST_CapsuleTrace		UMETA(DisplayName = "CapsuleTrace"),
	TST_BoxTrace		UMETA(DisplayName = "BoxTrace")
};

UENUM(BlueprintType)
enum class EGASXTraceHitType : uint8
{
	THT_SingleTrace		UMETA(DisplayName = "Single"),
	THT_MultiTrace		UMETA(DisplayName = "Multi")
};

/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Abstract, Blueprintable, meta = (ShowWorldContextPin))
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXTargetType : public UObject
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UGASXTargetType() {}

	UFUNCTION(BlueprintPure, Category = "GASXTargetType")
	UObject* GetWorldContextObjectFromActorInfo(FGameplayAbilityActorInfo ActorInfo) const;

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent, Category = "GASXTargetType")
	void GetTargets(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner actor */
UCLASS(NotBlueprintable)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXTargetType_UseOwner : public UGASXTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGASXTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that uses the avatar actor */
UCLASS(NotBlueprintable)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXTargetType_UseAvatar : public UGASXTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGASXTargetType_UseAvatar() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXTargetType_UseEventData : public UGASXTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGASXTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls targets with a sphere trace */
UCLASS(Abstract)
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXTargetType_TraceBase : public UGASXTargetType
{
	GENERATED_BODY()

public:
	// Sphere trace type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	EGASXTraceTargetType TraceTargetType = EGASXTraceTargetType::TTT_ByChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	EGASXTraceShapeType TraceShapeType = EGASXTraceShapeType::TST_LineTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	EGASXTraceHitType TraceHitType = EGASXTraceHitType::THT_SingleTrace;

	// Used for util function like GetSceneObjectTransform. By default, this is NOT used for trace, and it is up to subclasses.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GASXTargetType|Trace")
	ESceneObjectType SceneObjectType = ESceneObjectType::SOT_AvatarActor;

	// if true, hit actors are set to OutActors in GetTargets function, ensuring no duplicates.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	bool bHitActorsAsTargets = true;

	// for sphere trace and capsule trace
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace|ShapeSettings")
	float TraceRadius = 0.f;

	// for capsule trace
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace|ShapeSettings")
	float CapsuleTraceHalfHeight = 0.f;

	// for box trace
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace|ShapeSettings")
	FVector BoxTraceHalfSize = FVector(0.f, 0.f, 0.f);

	// for box trace
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace|ShapeSettings")
	FRotator BoxTraceOrientation = FRotator(0.f, 0.f, 0.f);

	// used for trace if TraceType is set to TT_ByChannel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = ETraceTypeQuery::TraceTypeQuery1;

	// used for trace if TraceType is set to TT_ByProfile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace", meta = (GetOptions = "GetCollisionProfileNames"))
	FName ProfileName;

	// used for trace if TraceType is set to TT_ByObjectTypes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	bool bTraceComplex = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	TEnumAsByte <EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::Type::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	FLinearColor DebugTraceColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	FLinearColor DebugTraceHitColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	float DebugDrawTime = 5.f;

public:
	// Constructor and overrides
	UGASXTargetType_TraceBase() {}

	// Gets an object from ActorInfo with SceneObjectType
	UFUNCTION(BlueprintPure, Category = "GASXTargetType|Trace")
	UObject* GetSceneObject(FGameplayAbilityActorInfo ActorInfo) const;

	// Gets transform of an object from ActorInfo with SceneObjectType
	UFUNCTION(BlueprintPure, Category = "GASXTargetType|Trace")
	FTransform GetSceneObjectTransform(FGameplayAbilityActorInfo ActorInfo) const;

	// Calculates trance start and end location.
	UFUNCTION(BlueprintNativeEvent, Category = "GASXTargetType|Trace")
	void GetTraceStartAndEnd(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, FVector& OutStart, FVector& OutEnd) const;

	// Gets actors to ignore for trace. By default, The avatar actor will be added.
	UFUNCTION(BlueprintNativeEvent, Category = "GASXTargetType|Trace")
	TArray<AActor*> GetActorsToIgnore(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData) const;

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

#if WITH_EDITOR
	UFUNCTION()
	static TArray<FName> GetCollisionProfileNames();
#endif
};

/** Trivial target type that pulls targets with a sphere trace from avatar actor to actor forward. */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API UGASXTargetType_TraceFromAvatarActor : public UGASXTargetType_TraceBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	FVector OffsetFromActor = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GASXTargetType|Trace")
	float TraceLength = 1.f;

public:
	void GetTraceStartAndEnd_Implementation(FGameplayAbilityActorInfo ActorInfo, FGameplayEventData EventData, FVector& OutStart, FVector& OutEnd) const override;
};