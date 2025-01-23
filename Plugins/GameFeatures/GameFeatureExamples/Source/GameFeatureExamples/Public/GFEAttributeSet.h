// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GASXAttributeSet.h"
#include "GFEAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class GAMEFEATUREEXAMPLES_API UGFEAttributeSet : public UGASXAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData TestValue1;
	ATTRIBUTE_ACCESSORS(UGFEAttributeSet, TestValue1)

	UPROPERTY(BlueprintReadOnly, Category = "MyAttributeSet")
	FGameplayAttributeData TestValue2;
	ATTRIBUTE_ACCESSORS(UGFEAttributeSet, TestValue2)

public:
	UGFEAttributeSet();
};
