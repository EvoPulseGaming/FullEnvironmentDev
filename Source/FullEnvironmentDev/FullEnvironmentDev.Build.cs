// 2017 - Community based open project

using UnrealBuildTool;

public class FullEnvironmentDev : ModuleRules
{
	public FullEnvironmentDev(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
