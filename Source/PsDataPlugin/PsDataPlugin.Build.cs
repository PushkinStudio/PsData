// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

using UnrealBuildTool;

public class PsDataPlugin : ModuleRules
{
	public PsDataPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"PsDataPlugin/Private",
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
