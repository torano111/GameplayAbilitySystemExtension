// Copyright 2024 Toranosuke Ichikawa

#include "MyHeroCharacter.h"
#include "MyPlayerState.h"
#include "MyAttributeSet.h"

AMyHeroCharacter::AMyHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UMyAttributeSet* AMyHeroCharacter::GetMyAttributeSet() const
{
	return MyAttributeSet.Get();
}

void AMyHeroCharacter::InitializeGAS(AController* NewController, AGASXPlayerState* NewPlayerState)
{
	Super::InitializeGAS(NewController, NewPlayerState);

	AMyPlayerState* PS = Cast<AMyPlayerState>(NewPlayerState);
	if (PS)
	{
		MyAttributeSet = PS->GetMyAttributeSet();
	}
}
