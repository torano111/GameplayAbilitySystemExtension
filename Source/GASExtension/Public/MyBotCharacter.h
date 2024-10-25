// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "MyBaseCharacter.h"
#include "MyBotCharacter.generated.h"

/**
 * Example AI controlled character
 */
UCLASS()
class GASEXTENSION_API AMyBotCharacter : public AMyBaseCharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class UGASXAbilitySystemComponent* ASCReference;

	UPROPERTY()
	class UMyAttributeSet* MyAttributeSetReference;

public:
	AMyBotCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
};
