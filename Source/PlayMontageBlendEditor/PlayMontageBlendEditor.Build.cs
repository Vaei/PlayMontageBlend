// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlayMontageBlendEditor : ModuleRules
{
	public PlayMontageBlendEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;
		// bUseUnity = false;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"PlayMontageBlend",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"BlueprintGraph",
			}
			);
	}
}
