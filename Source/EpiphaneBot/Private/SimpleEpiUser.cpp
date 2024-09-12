// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleEpiUser.h"
#include "EpiGameSettings.h"

USimpleEpiUser* USimpleEpiUser::Get(UObject* WorldContextObject, FString Name, int64 ID, FLinearColor InColor, bool CreateIfNotFound)
{
	USimpleEpiUser* User = NewObject<USimpleEpiUser>(WorldContextObject);
	User->DataSource = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (ensure(User->DataSource))
	{
		if (!User->DataSource->Exists(ID))
		{
			if (!CreateIfNotFound || !User->DataSource->Create(ID, Name))
			{
				return nullptr;
			}
		}

		if (InColor != FLinearColor::White)
		{
			User->DataSource->SetColor(ID, InColor);
		}

		if (User->DataSource->GetUserData(ID, User->Data))
		{
#define BIND_PROPERTY_CHANGED(Property) F ## Property ## ChangedDelegate Property ## Delegate; \
	Property ## Delegate.BindDynamic(User, &USimpleEpiUser::On ## Property ## Changed); \
	User->DataSource->BindOn ## Property ## Changed(ID, Property ## Delegate);

			BIND_PROPERTY_CHANGED(Caterium);
			BIND_PROPERTY_CHANGED(Prestige);
			BIND_PROPERTY_CHANGED(Color);
			BIND_PROPERTY_CHANGED(Avatar);

#undef BIND_PROPERTY_CHANGED

			return User;
		}
	}

    return nullptr;
}

USimpleEpiUser* USimpleEpiUser::GetUserFromName(UObject* WorldContextObject, FString Name)
{
	UEpiUserDataSubsystem* DataSource = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEpiUserDataSubsystem>();
	if (!ensure(DataSource))
	{
		return nullptr;
	}

	Name.RemoveFromStart(TEXT("@"));
	int32 ID = DataSource->GetIdForName(Name);
	if (ID == -1)
	{
		return nullptr;
	}

	return Get(WorldContextObject, Name, ID, FLinearColor::White, false);
}

USimpleEpiUser* USimpleEpiUser::GetUserFromAuthor(UObject* WorldContextObject, FTwitchMessageAuthor Author, bool CreateIfNotFound)
{
	return Get(WorldContextObject, Author.Name, FCString::Atoi(*Author.UserId), Author.Color, CreateIfNotFound);
}

UChatAvatar* USimpleEpiUser::GetAvatar_Implementation() const
{
	if (!IsValid(Data.Avatar))
	{
		return GetDefault<UEpiGameSettings>()->DefaultAvatar.LoadSynchronous();
	}
	return Data.Avatar;
}

int32 USimpleEpiUser::GetID_Implementation() const
{
    return Data.ID;
}

FString USimpleEpiUser::GetUserName_Implementation() const
{
	return Data.Name;
}

FLinearColor USimpleEpiUser::GetUserColor_Implementation() const
{
	return Data.Color;
}

int32 USimpleEpiUser::GetCaterium_Implementation() const
{
	return Data.Caterium;
}

int32 USimpleEpiUser::GetPrestige_Implementation() const
{
	return Data.Prestige;
}

void USimpleEpiUser::AddCaterium_Implementation(int32 Caterium)
{
	return DataSource->AddCaterium(Data.ID, Caterium);
}

void USimpleEpiUser::LockCaterium_Implementation(int32 Amount)
{
	return DataSource->LockCaterium(Data.ID, Amount);
}

void USimpleEpiUser::UnlockCaterium_Implementation()
{
	return DataSource->UnlockCaterium(Data.ID);
}

void USimpleEpiUser::ForefeitLockedCaterium_Implementation()
{
	return DataSource->ForefeitLockedCaterium(Data.ID);
}

void USimpleEpiUser::GiveCaterium_Implementation(const TScriptInterface<IEpiUser>& Other, int32 Amount)
{
	if (!ensure(Other))
	{
		return;
	}

	if (Execute_GetCaterium(this) < Amount)
	{
		return;
	}

	Execute_AddCaterium(this, -Amount);
	Other->Execute_AddCaterium(Other.GetObject(), Amount);
}

void USimpleEpiUser::SetAvatar_Implementation(UChatAvatar* NewAvatar) const
{
	return DataSource->SetAvatar(Data.ID, NewAvatar);
}

void USimpleEpiUser::BindOnCateriumChanged_Implementation(const FOnUserCateriumChangedDelegate& Callback)
{
	OnCateriumChangedDelegate.Add(Callback);
}

void USimpleEpiUser::BindOnPrestigeChanged_Implementation(const FOnUserPrestigeChangedDelegate& Callback)
{
	OnPrestigeChangedDelegate.Add(Callback);
}

void USimpleEpiUser::BindOnAvatarChanged_Implementation(const FOnUserAvatarChangedDelegate& Callback)
{
	OnAvatarChangedDelegate.Add(Callback);
}

void USimpleEpiUser::OnColorChanged(int32 ID, FLinearColor NewColor)
{
	Data.Color = NewColor;
	OnColorChangedDelegate.Broadcast(NewColor);
}

void USimpleEpiUser::OnCateriumChanged(int32, int32 NewCaterium)
{
	Data.Caterium = NewCaterium;
	OnCateriumChangedDelegate.Broadcast(NewCaterium);
}

void USimpleEpiUser::OnPrestigeChanged(int32, int32 NewPrestige)
{
	Data.Prestige = NewPrestige;
	OnPrestigeChangedDelegate.Broadcast(NewPrestige);
}

void USimpleEpiUser::OnAvatarChanged(int32 ID, UChatAvatar* NewAvatar)
{
	Data.Avatar = NewAvatar;
	OnAvatarChangedDelegate.Broadcast(NewAvatar);
}
