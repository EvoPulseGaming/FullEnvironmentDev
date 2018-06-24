// 2017 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class FullEnvironmentDevTarget : TargetRules
{
	public FullEnvironmentDevTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.AddRange( new string[] { "FullEnvironmentDev" } );
	}
}
