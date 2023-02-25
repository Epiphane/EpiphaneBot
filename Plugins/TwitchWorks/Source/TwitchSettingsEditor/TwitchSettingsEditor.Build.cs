// Copyright 2017-2022 HowToCompute. All Rights Reserved.

using UnrealBuildTool;

public class TwitchSettingsEditor : ModuleRules
{
	public TwitchSettingsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "TwitchSettingsPlugin",
			}
			);
	}
}
