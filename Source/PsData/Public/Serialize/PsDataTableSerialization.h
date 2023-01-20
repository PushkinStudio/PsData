// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "Collection/PsDataMapProxy.h"
#include "PsDataDefines.h"
#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/DataTable.h"

class UPsData;

/***********************************
 * FPsDataTableSerializer
 ***********************************/

struct PSDATA_API FPsDataTableSerializer
{
	static void CreateTableFromJson_Import(UDataTable* DataTable, const TArray<TSharedPtr<FJsonValue>>& JsonArray, TArray<FString>& ImportProblems);

private:
	static void ReportAboutExtraFields(const UScriptStruct* Struct, FName RowName, const TSharedPtr<FJsonObject>& JsonObject, TArray<FString>& ImportProblems);
};

/***********************************
 * FPsDataTableDeserializer
 ***********************************/

struct PSDATA_API FPsDataTableDeserializer : public FPsDataDeserializer
{
private:
	FPsDataJsonDeserializer JsonDeserializer;

public:
	FPsDataTableDeserializer(UDataTable* DataTable, const FString& PropertyName);

	template <typename T>
	FPsDataTableDeserializer(UDataTable* DataTable, TPsDataConstMapProxy<T> MapProxy)
		: FPsDataTableDeserializer(DataTable, MapProxy.GetField()->GetNameForSerialize())
	{
	}

	template <typename T>
	FPsDataTableDeserializer(UDataTable* DataTable, TPsDataMapProxy<T> MapProxy)
		: FPsDataTableDeserializer(DataTable, MapProxy.GetField()->GetNameForSerialize())
	{
	}

	virtual ~FPsDataTableDeserializer(){};

public:
	virtual bool ReadKey(FString& OutKey) override;
	virtual bool ReadIndex() override;
	virtual bool ReadArray() override;
	virtual bool ReadObject() override;
	virtual bool ReadValue(int32& OutValue) override;
	virtual bool ReadValue(int64& OutValue) override;
	virtual bool ReadValue(uint8& OutValue) override;
	virtual bool ReadValue(float& OutValue) override;
	virtual bool ReadValue(bool& OutValue) override;
	virtual bool ReadValue(FString& OutValue) override;
	virtual bool ReadValue(FName& OutValue) override;
	virtual bool ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator) override;

	virtual void PopKey(const FString& Key) override;
	virtual void PopIndex() override;
	virtual void PopArray() override;
	virtual void PopObject() override;

	/***********************************
	 * Table serialize
	 ***********************************/

	static TSharedPtr<FJsonObject> CreateJsonFromTable(UDataTable* DataTable, const FString& PropertyName);
	static TArray<TSharedPtr<FJsonValue>> CreateJsonFromTable_Export(UDataTable* DataTable);
};
