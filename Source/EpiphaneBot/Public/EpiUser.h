// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EpiUserDataSubsystem.h"
#include "ChatAvatar.h"
#include "EpiUser.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserCateriumChangedDelegate, int32, NewCaterium);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserCateriumChangedDelegate, int32, NewCaterium);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserPrestigeChangedDelegate, int32, NewPrestige);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserPrestigeChangedDelegate, int32, NewPrestige);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserColorChangedDelegate, FLinearColor, NewColor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUserColorChangedDelegate, FLinearColor, NewColor);

UINTERFACE(BlueprintType)
class EPIPHANEBOT_API UEpiUser : public UInterface
{
	GENERATED_BODY()
};

class EPIPHANEBOT_API IEpiUser
{
	GENERATED_BODY()

public:
	void Init(UObject* WorldContextObject, int32 ID);
	bool ReloadData();

protected:
	virtual void Initialize(int32 ID) { PURE_VIRTUAL(ID); }

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetID() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FString GetUserName() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FLinearColor GetUserColor() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetCaterium() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetPrestige() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddCaterium(int32 Caterium);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LockCaterium(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnlockCaterium();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ForefeitLockedCaterium();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GiveCaterium(const TScriptInterface<IEpiUser>& Other, int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindOnColorChanged(const FColorChangedDelegate& Callback);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindOnCateriumChanged(const FOnUserCateriumChangedDelegate& Callback);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindOnPrestigeChanged(const FOnUserPrestigeChangedDelegate& Callback);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UChatAvatar* GetAvatar() const;

protected:
	FEpiUserData Data;

	UEpiUserDataSubsystem* UserData;
};

UCLASS()
class EPIPHANEBOT_API UEpiUserObject : public UObject, public IEpiUser
{
	GENERATED_BODY()
	
protected:	
	virtual void Initialize(int32 ID) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Epi User", meta = (WorldContext = "WorldContextObject"))
	static bool Exists(UObject* WorldContextObject, int64 Id);

	UFUNCTION(BlueprintCallable, Category = "Epi User", meta = (DisplayName = "Find By Name", AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static TScriptInterface<IEpiUser> Find(UObject* Outer, FString Name, UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "Epi User", meta = (DisplayName = "Find By ID", AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static TScriptInterface<IEpiUser> FindById(UObject* Outer, int64 Id, UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "Epi User", meta = (AutoCreateRefTerm = "Class", WorldContext = "WorldContextObject"))
	static TScriptInterface<IEpiUser> Get(UObject* Outer, int64 Id, FString Name, UClass* Class);

public:
	UFUNCTION()
	void OnColorChanged(int32 ID, FLinearColor NewColor);

	UFUNCTION()
	void OnCateriumChanged(int32 ID, int32 NewCaterium);

	UFUNCTION()
	void OnPrestigeChanged(int32 ID, int32 NewPrestige);

protected:
	UPROPERTY(BlueprintAssignable)
	FUserColorChangedDelegate OnColorChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUserCateriumChangedDelegate OnCateriumChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUserPrestigeChangedDelegate OnPrestigeChangedDelegate;
};
