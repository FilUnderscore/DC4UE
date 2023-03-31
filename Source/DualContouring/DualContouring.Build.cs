using UnrealBuildTool;

public class DualContouring : ModuleRules
{
	public DualContouring(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "RuntimeMeshComponent", "RenderCore", "RHI" });
      //PrivateIncludePaths.AddRange(new string[] { "DualContouring/Private" ]});
    }
}