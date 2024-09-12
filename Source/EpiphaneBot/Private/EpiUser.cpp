// Fill out your copyright notice in the Description page of Project Settings.

#include "EpiUser.h"
#include "EpiUserDataSubsystem.h"

void IEpiUser::Init(UObject* WorldContextObject, int32 ID)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (World)
	{
		UGameInstance* GameInstance = World->GetGameInstance();
		if (GameInstance)
		{
			UserData = GameInstance->GetSubsystem<UEpiUserDataSubsystem>();
			ensure(IsValid(UserData));
		}
	}

	Initialize(ID);
}

bool IEpiUser::ReloadData()
{
	if (ensure(UserData))
	{
		return UserData->GetUserData(Data.ID, Data);
	}

	return false;
}

void UEpiUserObject::Initialize(int32 ID)
{
	if (ensure(IsValid(UserData)))
	{
#define BIND_PROPERTY_CHANGED(Property) F ## Property ## ChangedDelegate Property ## Delegate; \
	Property ## Delegate.BindDynamic(this, &UEpiUserObject::On ## Property ## Changed); \
	UserData->BindOn ## Property ## Changed(Data.ID, Property ## Delegate);

		BIND_PROPERTY_CHANGED(Caterium);
		BIND_PROPERTY_CHANGED(Prestige);
		BIND_PROPERTY_CHANGED(Color);

#undef BIND_PROPERTY_CHANGED
	}
}

bool UEpiUserObject::Exists(UObject* WorldContextObject, int64 ID)
{
	UEpiUserDataSubsystem* UserData = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (!ensure(IsValid(UserData)))
	{
		return false;
	}

	return UserData->Exists(ID);
}

TScriptInterface<IEpiUser> UEpiUserObject::Find(UObject* WorldContextObject, FString Name, UClass* Class)
{
	UEpiUserDataSubsystem* UserData = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (!ensure(IsValid(UserData)))
	{
		return nullptr;
	}

	Name.RemoveFromStart(TEXT("@"));

	int id = UserData->GetIdForName(Name);
	if (id < 0)
	{
		return nullptr;
	}

	return FindById(WorldContextObject, id, Class);
}

TScriptInterface<IEpiUser> UEpiUserObject::FindById(UObject* Outer, int64 ID, UClass* Class)
{
	if (!Exists(Outer, ID))
	{
		return nullptr;
	}

	UEpiUser* ChatPlayer = NewObject<UEpiUser>(Outer, Class);
	IEpiUser* Interfaced = Cast<IEpiUser>(ChatPlayer);
	Interfaced->Init(Outer, ID);
	if (Interfaced->ReloadData())
	{
		return ChatPlayer;
	}

	return nullptr;
}

TScriptInterface<IEpiUser> UEpiUserObject::Get(UObject* Outer, int64 ID, FString Name, UClass* Class)
{
	if (!ensure(Class && Class->ImplementsInterface(UEpiUser::StaticClass())))
	{
		return nullptr;
	}

	UEpiUserObject* ChatPlayer = NewObject<UEpiUserObject>(Outer, Class);
	if (ensure(ChatPlayer->UserData))
	{
		if (!ChatPlayer->UserData->Exists(ID) && 
			!ChatPlayer->UserData->Create(ID, Name))
		{
			return nullptr;
		}

		if (ChatPlayer->UserData->GetUserData(ID, ChatPlayer->Data))
		{
			return ChatPlayer;
		}
	}

	return nullptr;
}

void UEpiUserObject::OnColorChanged(int32 ID, FLinearColor NewColor)
{
	Data.Color = NewColor;
	OnColorChangedDelegate.Broadcast(NewColor);
}

void UEpiUserObject::OnAvatarChanged(int32 ID, UChatAvatar* NewAvatar)
{
	Data.Avatar = NewAvatar;
	OnAvatarChangedDelegate.Broadcast(NewAvatar);
}

void UEpiUserObject::OnCateriumChanged(int32, int32 NewCaterium)
{
	Data.Caterium = NewCaterium;
	OnCateriumChangedDelegate.Broadcast(NewCaterium);
}

void UEpiUserObject::OnPrestigeChanged(int32, int32 NewPrestige)
{
	Data.Prestige = NewPrestige;
	OnPrestigeChangedDelegate.Broadcast(NewPrestige);
}
