// Copyright 2024 Toranosuke Ichikawa

#include "MyPlayerState.h"
#include "MyAttributeSet.h"

AMyPlayerState::AMyPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MyAttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("MyAttributeSet"));
}

UMyAttributeSet* AMyPlayerState::GetMyAttributeSet() const
{
	return MyAttributeSet;
}
