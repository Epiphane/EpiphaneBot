// Copyright 2017-2022 HowToCompute. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTwitchWorks, Verbose, All);

class FTwitchPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	// This variable stored the OAuth token of the latest successful login attempt.
	// It is used as an iterim solution to bypass Helix requiring API tokens for all requests, while some old Kraken ones did not -> the new Chat must be passed in requirement
	static FString LastOAuthToken;
};