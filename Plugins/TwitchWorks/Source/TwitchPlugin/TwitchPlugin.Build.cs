// Copyright 2017-2022 HowToCompute. All Rights Reserved.

using UnrealBuildTool;

public class TwitchPlugin : ModuleRules
{
	public TwitchPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"WebSockets",
				"HTTPServer"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "HTTP",
                "Json",
                "JsonUtilities",
                "RenderCore",
                "ImageWrapper",
                "RHI",
                "TwitchSettingsPlugin",
			}
			);
	}
}
