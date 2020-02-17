// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataTableSerialization.h"

#include "PsData.h"
#include "PsDataCore.h"
#include "Serialize/PsDataStructSerialization.h"

#include "JsonObjectConverter.h"

FPsDataTableDeserializer::FPsDataTableDeserializer(UDataTable* DataTable, const FString& PropertyName)
	: FPsDataDeserializer()
	, JsonDeserializer(CreateJsonFromTable(DataTable, PropertyName))
{
	check(DataTable);
}

bool FPsDataTableDeserializer::ReadKey(FString& OutKey)
{
	return JsonDeserializer.ReadKey(OutKey);
}

bool FPsDataTableDeserializer::ReadIndex()
{
	return JsonDeserializer.ReadIndex();
}

bool FPsDataTableDeserializer::ReadArray()
{
	return JsonDeserializer.ReadArray();
}

bool FPsDataTableDeserializer::ReadObject()
{
	return JsonDeserializer.ReadObject();
}

bool FPsDataTableDeserializer::ReadValue(int32& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataTableDeserializer::ReadValue(uint8& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataTableDeserializer::ReadValue(float& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataTableDeserializer::ReadValue(bool& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataTableDeserializer::ReadValue(FString& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataTableDeserializer::ReadValue(FName& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataTableDeserializer::ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator)
{
	return JsonDeserializer.ReadValue(OutValue, Allocator);
}

void FPsDataTableDeserializer::PopKey(const FString& Key)
{
	return JsonDeserializer.PopKey(Key);
}

void FPsDataTableDeserializer::PopIndex()
{
	return JsonDeserializer.PopIndex();
}

void FPsDataTableDeserializer::PopArray()
{
	return JsonDeserializer.PopArray();
}

void FPsDataTableDeserializer::PopObject()
{
	return JsonDeserializer.PopObject();
}

/***********************************
 * DataTable serialize
 ***********************************/

TSharedPtr<FJsonObject> FPsDataTableDeserializer::CreateJsonFromTable(UDataTable* DataTable, const FString& PropertyName)
{
	TSharedPtr<FJsonObject> RootJsonObject(new FJsonObject());

	const UScriptStruct* Struct = DataTable->GetRowStruct();
	check(Struct);
	const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
	if (RowMap.Num() > 0)
	{
		TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
		for (auto& Pair : RowMap)
		{
			JsonObject->SetObjectField(Pair.Key.ToString().ToLower(), FPsDataStructDeserializer::CreateJsonFromStruct(Struct, Pair.Value));
		}
		RootJsonObject->SetObjectField(PropertyName, JsonObject);
	}

	/*
	FString JsonString;
	auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);
	UE_LOG(LogData, Warning, TEXT("Json: %s"), *JsonString);
	*/

	return RootJsonObject;
}
