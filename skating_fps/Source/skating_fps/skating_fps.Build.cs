using UnrealBuildTool;

public class skating_fps : ModuleRules
{
	public skating_fps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ "Core", "CoreUObject", "Engine", "InputCore", "Networking",
		  "OnlineSubsystem", "OnlineSubsystemNull", "OnlineSubsystemUtils", "Niagara", "PhysicsCore",
		  "AkAudio"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
		
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
