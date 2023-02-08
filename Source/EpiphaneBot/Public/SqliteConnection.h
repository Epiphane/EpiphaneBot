// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "sqlite3.h"
#include "SqliteConnection.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSqliteConnection, Log, All);

/**
 *
 */
USTRUCT()
struct FTemporarySqliteConnection
{
	GENERATED_BODY()

public:
	FTemporarySqliteConnection(USqliteConnection* Connection = nullptr);
	~FTemporarySqliteConnection();

	USqliteConnection& operator*() const { return *Connection; }
	USqliteConnection* operator->() const { return Connection; }

	operator bool() const { return IsValid(); }
	bool IsValid() const { return Connection != nullptr; }

private:
	UPROPERTY()
	USqliteConnection* Connection;
};

UENUM()
enum ESqliteValueType
{
	Integer = 0,
	Float,
	Text,
	UnsupportedValueType
};

USTRUCT()
struct FSQLiteValue
{
	GENERATED_BODY()
	
	UPROPERTY()
	TEnumAsByte<ESqliteValueType> Type;

	UPROPERTY()
	FString StringValue;

	UPROPERTY()
	double DoubleValue;

	UPROPERTY()
	int64 IntValue;
};

UENUM()
enum ESqliteStepResult
{
	Done = 0,
	Data,
	Error,
};

/**
 *
 */
USTRUCT()
struct FSqliteStatement
{
	GENERATED_BODY()

public:
	FSqliteStatement(USqliteConnection* Connection = nullptr, sqlite3_stmt* Statement = nullptr);
	~FSqliteStatement();

	operator bool() const { return IsValid(); }
	bool IsValid() const { return Statement != nullptr; }

public:
	bool Bind(int Parameter, int64 IntVal);
	bool Bind(int Parameter, FString StringVal);
	
	ESqliteStepResult Step();
	TMap<FString, FSQLiteValue> ReadRow();
	bool AssignNextRowToObject(UObject* Object);

private:
	UPROPERTY()
	USqliteConnection* Connection;

	sqlite3_stmt* Statement = nullptr;
};

/**
 * 
 */
UCLASS()
class EPIPHANEBOT_API USqliteConnection : public UObject
{
	GENERATED_BODY()
	
public:
	static [[nodiscard]] FTemporarySqliteConnection Open(FString DatabasePath);
	static [[nodiscard]] FTemporarySqliteConnection OpenDefault();

	~USqliteConnection();
	void Close();

public:
	bool Execute(FString query);
	FSqliteStatement [[nodiscard]] Prepare(FString query);

private:
	friend struct FSqliteStatement;

	sqlite3* Database;
};
