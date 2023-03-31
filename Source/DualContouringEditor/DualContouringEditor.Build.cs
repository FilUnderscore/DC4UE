using UnrealBuildTool;

public class DualContouringEditor : ModuleRules
{
	public DualContouringEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Slate", "SlateCore", "Engine", "DualContouring", "UnrealEd", "PropertyEditor" });
    }
}