// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Collection/PsDataMapProxy.h"
#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/DataTable.h"

class UPsData;

/***********************************
 * FTableDataSerializer
 ***********************************/

struct PSDATA_API FPsDataTableDeserializer : public FPsDataDeserializer
{
private:
	FPsDataJsonDeserializer JsonDeserializer;

public:
	FPsDataTableDeserializer(UDataTable* DataTable, const FString& PropertyName);

	template <typename T, bool bConst>
	FPsDataTableDeserializer(UDataTable* DataTable, const FPsDataBaseMapProxy<T, bConst>& MapProxy)
		: FPsDataTableDeserializer(DataTable, MapProxy.GetField()->Name)
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

private:
	/***********************************
	 * DataTable serialize
	 ***********************************/

	TSharedPtr<FJsonObject> CreateJsonFromTable(UDataTable* DataTable, const FString& PropertyName);
};
