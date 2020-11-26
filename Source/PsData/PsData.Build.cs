// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

using UnrealBuildTool;

public class PsData : ModuleRules
{
	public PsData(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"PsData/Private",
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Json",
				"JsonUtilities",
			}
		);
	}
}
