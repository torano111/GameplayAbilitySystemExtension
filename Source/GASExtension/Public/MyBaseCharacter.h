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

protected:
	TWeakObjectPtr<class UMyAttributeSet> MyAttributeSet;

public:
	AMyBaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "MyBaseCharacter")
	class UMyAttributeSet* GetMyAttributeSet() const;

protected:
	virtual void OnGASXPawnComponentInitialized(UGASXAbilitySystemComponent* InAbilitySystemComponent, AActor* InOwnerActor, AActor* InAvatarActor, class AGASXPlayerState* InPlayerState) override;
};
