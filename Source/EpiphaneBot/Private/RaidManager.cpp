// Fill out your copyright notice in the Description page of Project Settings.


#include "RaidManager.h"
#include "AssetRegistry/AssetRegistryModule.h"

DEFINE_LOG_CATEGORY(LogRaidManager);

void URaidManager::Initialize(FSubsystemCollectionBase& Collection)
{
    // Load the asset registry module
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
    // The asset registry is populated asynchronously at startup, so there's no guarantee it has finished.
    // This simple approach just runs a synchronous scan on the entire content directory.
    // Better solutions would be to specify only the path to where the relevant blueprints are,
    // or to register a callback with the asset registry to be notified of when it's finished populating.
    TArray<FString> ContentPaths;
    ContentPaths.Add(TEXT("/Game"));
    AssetRegistry.ScanPathsSynchronous(ContentPaths);

    FName BaseClassName = URaidEvent::StaticClass()->GetFName();

    // Use the asset registry to get the set of all class names deriving from Base
    TSet<FName> DerivedNames;
    {
        TArray<FName> BaseNames;
        BaseNames.Add(BaseClassName);

        TSet<FName> Excluded;
        AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);
    }

    FARFilter Filter;
    Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
    Filter.bRecursiveClasses = true;
    Filter.bRecursivePaths = true;

    TArray<FAssetData> AssetList;
    AssetRegistry.GetAssets(Filter, AssetList);

    // Iterate over retrieved blueprint assets
    for (auto const& Asset : AssetList)
    {
        // Get the the class this blueprint generates (this is stored as a full path)
        if (FAssetTagValueRef GeneratedClassPathPtr = Asset.TagsAndValues.FindTag(TEXT("GeneratedClass")); GeneratedClassPathPtr.IsSet())
        {
            // Convert path to just the name part
            const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(*GeneratedClassPathPtr.AsExportPath().ToString());
            const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);

            // Check if this class is in the derived set
            if (!DerivedNames.Contains(*ClassName))
            {
                continue;
            }

            // Store using the path to the generated class
            AvailableEvents.Add(TSoftObjectPtr<UClass>(FStringAssetReference(ClassObjectPath)).LoadSynchronous());
        }
    }
}

bool URaidManager::IsRaidOngoing()
{
    if (!CurrentRaid)
    {
        return false;
    }

    ERaidState State = CurrentRaid->GetState();
    if (State == ERaidState::Done)
    {
        CurrentRaid = nullptr;
    }

    switch (State)
    {
    case ERaidState::Done:
    case ERaidState::NotStarted:
        return false;
    case ERaidState::Preparing:
    case ERaidState::Running:
    default:
        return true;
    }
}

ARaid* URaidManager::StartRaid(TSubclassOf<ARaid> RaidClass)
{
    check(CurrentRaid == nullptr);

    CurrentRaid = ARaid::CreateRaid(GetWorld(), RaidClass, AvailableEvents, Chat);
    CurrentRaid->GetOnRaidCompleteDelegate().AddDynamic(this, &URaidManager::OnRaidDone);
    CurrentRaid->BeginPreparing();
    return CurrentRaid;
}

ARaid* URaidManager::GetCurrentRaid()
{
    if (CurrentRaid && CurrentRaid->GetState() == ERaidState::Done)
    {
        CurrentRaid = nullptr;
    }

    return CurrentRaid;
}

void URaidManager::GetRaidEligibility(AChatPlayer* Player, ERaidEligibility& Result, ARaid*& Raid, URaidParticipantComponent*& Participant, float& Cooldown)
{
    Raid = CurrentRaid;
    Cooldown = GetWorld()->GetTimerManager().GetTimerRemaining(RaidCooldownTimer);
    if (Cooldown > 0)
    {
        Result = ERaidEligibility::Cooldown;
    }
    else if (CurrentRaid == nullptr)
    {
        Result = ERaidEligibility::NoCurrentRaid;
    }
    else
    {
        ERaidState state = CurrentRaid->GetState();
        EJoinableOutput Joinable;
        CurrentRaid->IsJoinable(Player, Joinable, Participant);
        if (Joinable == EJoinableOutput::RaidIsJoinable)
        {
            Result = ERaidEligibility::RaidIsJoinable;
        }
        else if(Joinable == EJoinableOutput::RaidNotJoinable)
        {
            Result = ERaidEligibility::RaidIsOngoing;
        }
        else if (Joinable == EJoinableOutput::AlreadyParticipating)
        {
            Result = ERaidEligibility::AlreadyParticipating;
        }
    }
}

void URaidManager::OnRaidDone(ARaid* Raid)
{
    CurrentRaid = nullptr;
    GetWorld()->GetTimerManager().SetTimer(RaidCooldownTimer, this, &URaidManager::OnCooldownOver, Raid->GetTimeBeforeNextRaid());
    UE_LOG(LogRaidManager, Log, TEXT("Raid complete"));
}

void URaidManager::OnCooldownOver()
{
    OnRaidCooldownOver.Broadcast();
}