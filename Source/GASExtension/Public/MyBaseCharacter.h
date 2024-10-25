// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXBaseCharacter.h"
#include "MyAttributeSet.h"
#include "MyBaseCharacter.generated.h"

/**
 * Example base character
 */
UCLASS()
class GASEXTENSION_API AMyBaseCharacter : public AGASXBaseCharacter
{
	GENERATED_BODY()

public:
	// Abilities which will be granted on startup, such as passive abilities. These won't be activated automatically.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyBaseCharacter")
	TArray<TSubclassOf<class UGASXGameplayAbility>> StartupAbilities;

	// Effects which will be applied on startup, such as attribute initializer.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MyBaseCharacter")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

protected:
	TWeakObjectPtr<class UMyAttributeSet> MyAttributeSet;

public:
	AMyBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "MyBaseCharacter")
	class UMyAttributeSet* GetMyAttributeSet() const;

	virtual void InitGameplayAbilitySystem(AActor* InOwnerActor, AActor* InAvatarActor, class AGASXPlayerState* NewPlayerState = nullptr) override;
};
