// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class PsDataEditor : ModuleRules
	{
		public PsDataEditor(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PrivateIncludePaths.AddRange(
				new string[] {
					"PsDataEditor/Private",
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"PsData",
					"Json",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"AnimGraph",
					"BlueprintGraph",
					"UnrealEd",
					"AssetTools",
					"UnrealEd",
					"KismetWidgets",
					"KismetCompiler",
					"GraphEditor",
					"Kismet",
					"PropertyEditor",
					"EditorStyle",
					"Sequencer",
					"DetailCustomizations",
					"Settings",
					"RenderCore",
					"Slate",
					"SlateCore",
				}
			);
		}
	}
}
