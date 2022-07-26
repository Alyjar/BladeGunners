using UnrealBuildTool;
using System.Collections.Generic;

public class skating_fpsTarget : TargetRules
{
	public skating_fpsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "skating_fps" } );
	}
}
