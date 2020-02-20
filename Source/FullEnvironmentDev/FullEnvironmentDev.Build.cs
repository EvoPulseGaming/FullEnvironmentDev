// 2017 - Community based open project

using UnrealBuildTool;

public class FullEnvironmentDev : ModuleRules
{
	public FullEnvironmentDev(ReadOnlyTargetRules Target) : base(Target)
	{
		//Our PrivatePCH that we want to globally #include
		PrivatePCHHeaderFile = "PrivatePCH.h";

		// Make sure UBT reminds us of how to keep the project IWYU compliant
		bEnforceIWYU = true;

		//Enable IWYU but keep our PrivatePCH in use
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
