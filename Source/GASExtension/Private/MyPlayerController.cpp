// Copyright 2024 Toranosuke Ichikawa

#include "MyPlayerController.h"
#include "GASXGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Experience/GASXExperienceManagerComponent.h"

bool AMyPlayerController::ShouldShowLoadingScreen(FString& OutReason) const
{
	auto bShowLoadingScreen = true;
	if (auto GM = Cast<AGASXGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		bShowLoadingScreen = !GM->ExperienceManagerComponent->IsExperienceLoaded();
		if (bShowLoadingScreen)
		{
			OutReason = "Experience is not loaded yet.";
		}
	}
	else
	{
		OutReason = "AGASXGameMode not found.";
	}

	return bShowLoadingScreen;
}
