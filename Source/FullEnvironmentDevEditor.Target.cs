// 2015 - Community based open project

using UnrealBuildTool;
using System.Collections.Generic;

public class FullEnvironmentDevEditorTarget : TargetRules
{
	public FullEnvironmentDevEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "FullEnvironmentDev" } );
	}
}
