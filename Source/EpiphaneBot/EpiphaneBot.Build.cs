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
            "Engine",
            "InputCore",
            "TwitchApi",
            "TwitchPlugin",
            "TwitchSettingsPlugin",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
