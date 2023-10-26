// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlayMontageBlend : ModuleRules
{
	public PlayMontageBlend(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"GameplayTasks",
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine", 
				"GameplayAbilities",
			}
			);
	}
}
