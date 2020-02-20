// 2017 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class FullEnvironmentDevTarget : TargetRules
{
	public FullEnvironmentDevTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Game;
		ExtraModuleNames.AddRange( new string[] { "FullEnvironmentDev" } );
	}
}
