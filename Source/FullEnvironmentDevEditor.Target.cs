// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class FullEnvironmentDevEditorTarget : TargetRules
{
	public FullEnvironmentDevEditorTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "FullEnvironmentDev" } );
	}
}
