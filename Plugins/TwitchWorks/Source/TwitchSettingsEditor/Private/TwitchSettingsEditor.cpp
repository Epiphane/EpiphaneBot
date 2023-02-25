// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "TwitchSettingsEditor.h"

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"

#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "Engine.h"

#include "TwitchRuntimeSettings.h"

#define LOCTEXT_NAMESPACE "FTwitchSettingsEditorModule"

void FTwitchSettingsEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Attempt to fetch the settings module
	ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (settingsModule)
		// Register Plugin Settings
		ISettingsSectionPtr SettingsSection = settingsModule->RegisterSettings("Project", "Plugins", "TwitchWorks",
			LOCTEXT("RuntimeGeneralSettingsName", "TwitchWorks"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "TwitchWorks Plugin Settings"),
			GetMutableDefault<UTwitchRuntimeSettings>()
		);

}

void FTwitchSettingsEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	// Unregister Plugin Settings
	if (UObjectInitialized())
	{
		// Attempt to fetch the settings module
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if(SettingsModule)
			SettingsModule->UnregisterSettings("Project", "Plugins", "TwitchWorks");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTwitchSettingsEditorModule, TwitchSettingsEditor)