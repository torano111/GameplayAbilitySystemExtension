// Copyright Epic Games, Inc. All Rights Reserved.

#include "Experience/GASXUserFacingExperienceDefinition.h"
#include "Engine/AssetManager.h"

const FString UGASXUserFacingExperienceDefinition::ExperienceOptionName = TEXT("Experience");

FString UGASXUserFacingExperienceDefinition::ConstructTravelURL() const
{
	auto ExperienceName = ExperienceID.PrimaryAssetName.ToString();
	auto Args = ExtraArgs;
	Args.Add(ExperienceOptionName, ExperienceName);

	FString CombinedExtraArgs = "";
	for (const auto& KVP : Args)
	{
		if (!KVP.Key.IsEmpty())
		{
			if (KVP.Value.IsEmpty())
			{
				CombinedExtraArgs += FString::Printf(TEXT("?%s"), *KVP.Key);
			}
			else
			{
				CombinedExtraArgs += FString::Printf(TEXT("?%s=%s"), *KVP.Key, *KVP.Value);
			}
		}
	}

	FString MapName = "";
	FAssetData MapAssetData;
	if (UAssetManager::Get().GetPrimaryAssetData(MapID, /*out*/ MapAssetData))
	{
		MapName = MapAssetData.PackageName.ToString();
	}

	return FString::Printf(TEXT("%s%s"),
		*MapName,
		*CombinedExtraArgs);
}
