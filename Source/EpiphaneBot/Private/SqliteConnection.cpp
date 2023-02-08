// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteConnection.h"

DEFINE_LOG_CATEGORY(LogSqliteConnection);

const FString kDatabaseName = TEXT("RPG");
const FString kDatabasePath = TEXT("Databases/RPG.db");

FTemporarySqliteConnection::FTemporarySqliteConnection(USqliteConnection* Connection)
	: Connection(Connection)
{}

FTemporarySqliteConnection::~FTemporarySqliteConnection()
{
	if (Connection)
	{
		Connection->Close();
		Connection = nullptr;
	}
}

FSqliteStatement::FSqliteStatement(USqliteConnection* Connection, sqlite3_stmt* Statement)
	: Connection(Connection)
	, Statement(Statement)
{}

FSqliteStatement::~FSqliteStatement()
{
	if (Statement)
	{
		sqlite3_finalize(Statement);
		Statement = nullptr;
	}
}

bool FSqliteStatement::Bind(int Parameter, int64 IntVal)
{
	check(Statement);
	check(Parameter >= 1);
	int rc = sqlite3_bind_int64(Statement, Parameter, IntVal);
	if (rc != SQLITE_OK)
	{
		const char* error = sqlite3_errmsg(Connection->Database);
		UE_LOG(LogSqliteConnection, Error, TEXT("Failed to bind int value: %s"), ANSI_TO_TCHAR(error));
		return false;
	}

	return true;
}

bool FSqliteStatement::Bind(int Parameter, FString StringVal)
{
	check(Statement);
	check(Parameter >= 1);
	int rc = sqlite3_bind_text(Statement, Parameter, TCHAR_TO_ANSI(*StringVal), -1, nullptr);
	if (rc != SQLITE_OK)
	{
		const char* error = sqlite3_errmsg(Connection->Database);
		UE_LOG(LogSqliteConnection, Error, TEXT("Failed to bind text value: %s"), ANSI_TO_TCHAR(error));
		return false;
	}

	return true;
}

ESqliteStepResult FSqliteStatement::Step()
{
	int rc = sqlite3_step(Statement);
	switch (rc)
	{
	case SQLITE_DONE:
		return ESqliteStepResult::Done;
	case SQLITE_ROW:
		return ESqliteStepResult::Data;
	default:
		const char* error = sqlite3_errmsg(Connection->Database);
		UE_LOG(LogSqliteConnection, Error, TEXT("Failed to bind text value: %s"), ANSI_TO_TCHAR(error));
		return ESqliteStepResult::Error;
	}
}

TMap<FString, FSQLiteValue> FSqliteStatement::ReadRow()
{
	check(Statement);
	TMap<FString, FSQLiteValue> Result;

	int32 columns = sqlite3_column_count(Statement);
	for (int32 c = 0; c < columns; c++)
	{
		int32 columnType = sqlite3_column_type(Statement, c);
		const char* columnName = sqlite3_column_name(Statement, c);
		FString columnNameStr = UTF8_TO_TCHAR(columnName);

		FSQLiteValue& value = Result.Add(UTF8_TO_TCHAR(columnName));
		switch (columnType)
		{
		case SQLITE_INTEGER:
			value.Type = ESqliteValueType::Integer;
			value.IntValue = sqlite3_column_int64(Statement, c);
			break;
		case SQLITE_TEXT:
			value.Type = ESqliteValueType::Text;
			value.StringValue = UTF8_TO_TCHAR(sqlite3_column_text(Statement, c));
			break;
		case SQLITE_FLOAT:
			value.Type = ESqliteValueType::Float;
			value.DoubleValue = sqlite3_column_double(Statement, c);
			break;
		case SQLITE_NULL:
		default:
			value.Type = ESqliteValueType::UnsupportedValueType;
			break;
		}
	}

	return Result;
}

bool FSqliteStatement::AssignNextRowToObject(UObject* Object)
{
	check(Statement);
	UClass* SourceObjectClass = Object->GetClass();
	TMap<FString, FProperty*> Props;
	for (TFieldIterator<FProperty> PropIt(SourceObjectClass, EFieldIteratorFlags::SuperClassFlags::IncludeSuper);
		PropIt; ++PropIt)
	{
		Props.Add(*PropIt->GetNameCPP(), *PropIt);
	}

	TMap<FString, FSQLiteValue> Properties = ReadRow();
	for (const auto [name, value] : Properties)
	{
		if (!Props.Contains(name))
		{
			UE_LOG(LogSqliteConnection, Warning, TEXT("Could not find a property to assign for sqlite column '%s'. Ignoring..."), *name);
			continue;
		}

		FProperty* targetProperty = Props[name];

		if (value.Type == ESqliteValueType::Float)
		{
			if (FDoubleProperty* doubleProp = CastField<FDoubleProperty>(targetProperty))
			{
				doubleProp->SetPropertyValue_InContainer(Object, value.DoubleValue);
			}
			if (FFloatProperty* floatProp = CastField<FFloatProperty>(targetProperty))
			{
				floatProp->SetPropertyValue_InContainer(Object, value.DoubleValue);
			}
		}
		else if (value.Type == ESqliteValueType::Integer)
		{
			if (FInt64Property* int64prop = CastField<FInt64Property>(targetProperty))
			{
				int64prop->SetPropertyValue_InContainer(Object, value.IntValue);
			}
			else if (FIntProperty* int32prop = CastField<FIntProperty>(targetProperty))
			{
				int32prop->SetPropertyValue_InContainer(Object, (int32)value.IntValue);
			}
			else if (FInt16Property* int16prop = CastField<FInt16Property>(targetProperty))
			{
				int16prop->SetPropertyValue_InContainer(Object, (int16)value.IntValue);
			}
			else if (FInt8Property* int8prop = CastField<FInt8Property>(targetProperty))
			{
				int8prop->SetPropertyValue_InContainer(Object, (int8)value.IntValue);
			}
			else if (FBoolProperty* boolProp = CastField<FBoolProperty>(targetProperty))
			{
				boolProp->SetPropertyValue_InContainer(Object, value.IntValue > 0);
			}
		}
		else if (value.Type == ESqliteValueType::Text)
		{
			if (FStrProperty* strProp = CastField<FStrProperty>(targetProperty))
			{
				strProp->SetPropertyValue_InContainer(Object, value.StringValue);
			}
		}
	}

	return true;
}

FTemporarySqliteConnection USqliteConnection::Open(FString DatabasePath)
{
	sqlite3* db;
	int rc = sqlite3_open(TCHAR_TO_ANSI(*DatabasePath), &db);
	if (rc != SQLITE_OK)
	{
		const char* error = sqlite3_errmsg(db);
		UE_LOG(LogSqliteConnection, Error, TEXT("Failed to open database connection: %s"), ANSI_TO_TCHAR(error));
		return FTemporarySqliteConnection(nullptr);
	}

	USqliteConnection* Connection = NewObject<USqliteConnection>();
	Connection->Database = db;

    return FTemporarySqliteConnection(Connection);
}

FTemporarySqliteConnection USqliteConnection::OpenDefault()
{
	return USqliteConnection::Open(FPaths::ProjectContentDir() + kDatabasePath);
}

USqliteConnection::~USqliteConnection()
{
	Close();
}

void USqliteConnection::Close()
{
	if (Database)
	{
		sqlite3_close(Database);
		Database = nullptr;
	}
}

bool USqliteConnection::Execute(FString query)
{
	check(Database);
	int rc = sqlite3_exec(Database, TCHAR_TO_ANSI(*query), nullptr, nullptr, nullptr);
	if (rc != SQLITE_OK)
	{
		const char* error = sqlite3_errmsg(Database);
		UE_LOG(LogSqliteConnection, Error, TEXT("Failed to execute query %s: %s"), *query, ANSI_TO_TCHAR(error));
	}

	return true;
}

FSqliteStatement USqliteConnection::Prepare(FString Query)
{
	check(Database);

	sqlite3_stmt* Statement;
	int rc = sqlite3_prepare_v2(Database, TCHAR_TO_ANSI(*Query), -1, &Statement, nullptr);
	if (rc != SQLITE_OK)
	{
		const char* error = sqlite3_errmsg(Database);
		UE_LOG(LogSqliteConnection, Error, TEXT("Failed to prepare query %s: %s"), *Query, ANSI_TO_TCHAR(error));
	}

	return FSqliteStatement(this, Statement);
}
