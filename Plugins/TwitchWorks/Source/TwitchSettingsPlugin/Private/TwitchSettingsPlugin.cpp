// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "TwitchSettingsPlugin.h"

#define LOCTEXT_NAMESPACE "FTwitchSettingsPluginModule"

void FTwitchSettingsPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FTwitchSettingsPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTwitchSettingsPluginModule, TwitchSettingsPlugin)