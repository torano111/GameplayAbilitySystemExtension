// Copyright 2024 Toranosuke Ichikawa

#include "GASXPlayerController.h"
#include "GASXPlayerState.h"
#include "GASXAbilitySystemComponent.h"

UGASXAbilitySystemComponent* AGASXPlayerController::GetGASXAbilitySystemComponent() const
{
	const AGASXPlayerState* PS = Cast<AGASXPlayerState>(PlayerState);
	return (PS ? PS->GetGASXAbilitySystemComponent() : nullptr);
}

void AGASXPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UGASXAbilitySystemComponent* ASC = GetGASXAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
