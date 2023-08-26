using UnrealBuildTool;

public class TwitchApi : ModuleRules
{
	public TwitchApi(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject",
			"Engine",
			"HTTP",
            "Json",
            "JsonUtilities",
			"Kismet",
			"TwitchPlugin",
            "TwitchSettingsPlugin",
            "WebSockets",
		});
	}
}
