// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASXInputComponent.h"

UGASXInputComponent::UGASXInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UGASXInputComponent::BindNativeActionBP(const UGASXInputConfig* InputConfig, FGameplayTag InputTag, ETriggerEvent TriggerEvent, UObject* Object, FGASXNativeActionDelegate Delegate, bool bLogIfNotFound)
{
	if (Delegate.IsBound())
	{
		BindNativeAction(InputConfig, InputTag, TriggerEvent, Object, Delegate.GetFunctionName(), bLogIfNotFound);
	}
}

void UGASXInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
