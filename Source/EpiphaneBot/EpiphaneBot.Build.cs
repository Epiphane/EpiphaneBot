// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class EpiphaneBot : ModuleRules
{
	public EpiphaneBot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "DesktopPlatform",
            "Engine",
            "HTTP",
            "InputCore",
            "Paper2D",
            "Slate",
            "TwitchApi",
            "TwitchPlugin",
            "TwitchSettingsPlugin",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
