using System.IO;

namespace UnrealBuildTool.Rules
{
	public class PsDataEditorPlugin : ModuleRules
	{
		public PsDataEditorPlugin(ReadOnlyTargetRules Target) : base(Target)
		{
			PrivateIncludePaths.AddRange(
				new string[] {
					"PsDataEditorPlugin/Private",
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"PsDataEditorPlugin",
					"PsDataPlugin"
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
