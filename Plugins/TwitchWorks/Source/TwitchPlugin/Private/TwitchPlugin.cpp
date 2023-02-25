// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#include "TwitchPlugin.h"

DEFINE_LOG_CATEGORY(LogTwitchWorks);

#define LOCTEXT_NAMESPACE "FTwitchPluginModule"

FString FTwitchPluginModule::LastOAuthToken = TEXT("");

void FTwitchPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Load the HTTP module upon startup: that way we shouldn't hit any exceptions for when we're getting the module, but it hasn't been loaded
	FModuleManager::Get().LoadModule(TEXT("HTTP"));

	// Ensure the WebSockets module is loaded, and load it if it isn't (isn't loaded by default in shipped builds -> can lead to fatal exceptions in built versions of the game)
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}
}

void FTwitchPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTwitchPluginModule, TwitchPlugin)