// Copyright 2024 Toranosuke Ichikawa

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ModularPlayerController.h"
#include "GASXPlayerController.generated.h"

/**
 * AGASXPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS()
class GAMEPLAYABILITYSYSTEMEXTENSION_API AGASXPlayerController : public AModularPlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "PlayerController")
	class UGASXAbilitySystemComponent* GetGASXAbilitySystemComponent() const;

	//~APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
};
