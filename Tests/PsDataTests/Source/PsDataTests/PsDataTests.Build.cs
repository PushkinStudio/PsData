// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

using UnrealBuildTool;

public class PsDataTests : ModuleRules
{
	public PsDataTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Json",
				"PsData"
			}
		);
	}
}
