// Fill out your copyright notice in the Description page of Project Settings.

#include "ApplicationSettings.h"
#include "DesktopPlatformModule.h"

UApplicationSettings::UApplicationSettings(const FObjectInitializer& ObjectInitializer)
{
	DatabasePath = FPaths::ProjectSavedDir() + TEXT("Databases/RPG.db");
}

UApplicationSettings* UApplicationSettings::GetApplicationSettings()
{
	return Cast<UApplicationSettings>(GEngine->GetGameUserSettings());
}

void UApplicationSettings::SelectNewDatabasePath()
{
	if (!ensure(GEngine && GEngine->GameViewport))
	{
		return;
	}

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!ensure(DesktopPlatform))
	{
		return;
	}

	TArray<FString> FileNames;
	if (DesktopPlatform->OpenFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		TEXT("Choose Database file"),
		FPaths::GetPath(DatabasePath),
		DatabasePath,
		TEXT("*.db"),
		EFileDialogFlags::None,
		FileNames) &&
		!FileNames.IsEmpty())
	{
		DatabasePath = FileNames[0];
		OnDatabasePathChanged.Broadcast(DatabasePath);
	}
}
