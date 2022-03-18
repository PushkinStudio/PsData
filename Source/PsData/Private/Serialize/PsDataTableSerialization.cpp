// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Serialize/PsDataTableSerialization.h"

#include "PsData.h"
#include "PsDataCore.h"
#include "Serialize/PsDataStructSerialization.h"

#include "JsonObjectConverter.h"

/***********************************
 * FPsDataTableSerializer
 ***********************************/

void FPsDataTableSerializer::CreateTableFromJson_Import(UDataTable* DataTable, const TArray<TSharedPtr<FJsonValue>>& JsonArray, TArray<FString>& ImportProblems)
{
	const FString KeyField = DataTable->ImportKeyField.IsEmpty() ? TEXT("Name") : DataTable->ImportKeyField;

	auto Struct = DataTable->RowStruct;
	check(Struct);

	DataTable->EmptyTable();
	const TMap<FName, uint8*>* ConstRowMap = &DataTable->GetRowMap();
	const auto RowMap = const_cast<TMap<FName, uint8*>*>(ConstRowMap);

	TMap<FString, FString> KeyMap;
	for (int32 Index = 0; Index < JsonArray.Num(); ++Index)
	{
		const TSharedPtr<FJsonValue>& JsonValue = JsonArray[Index];
		TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
		if (!JsonObject.IsValid())
		{
			ImportProblems.Add(FString::Printf(TEXT("Row '%d' is not a valid JSON object."), Index));
			continue;
		}

		FName RowName = DataTableUtils::MakeValidName(JsonObject->GetStringField(KeyField));
		if (RowName.IsNone())
		{
			ImportProblems.Add(FString::Printf(TEXT("Row '%d' missing key field '%s'."), Index, *KeyField));
			continue;
		}

		JsonObject->RemoveField(KeyField);

		if (!DataTable->AllowDuplicateRowsOnImport() && RowMap->Contains(RowName))
		{
			ImportProblems.Add(FString::Printf(TEXT("Duplicate row name '%s'."), *RowName.ToString()));
			continue;
		}

		if (!DataTable->bIgnoreExtraFields)
		{
			ReportAboutExtraFields(Struct, RowName, JsonObject, ImportProblems);
		}

		auto RowStruct = FPsDataStructSerializer::CreateStructFromJson_Import(Struct, JsonObject.ToSharedRef(), ImportProblems);
		RowMap->Add(RowName, RowStruct);
	}
}

void FPsDataTableSerializer::ReportAboutExtraFields(const UScriptStruct* Struct, FName RowName, const TSharedPtr<FJsonObject>& JsonObject, TArray<FString>& ImportProblems)
{
	TArray<FString> Names;
	for (const auto& Pair : JsonObject->Values)
	{
		FName PropName = DataTableUtils::MakeValidName(Pair.Key);
		FProperty* ColumnProp = Struct->FindPropertyByName(PropName);
		for (TFieldIterator<FProperty> It(Struct); It && !ColumnProp; ++It)
		{
			Names.Append(DataTableUtils::GetPropertyImportNames(*It));
			ColumnProp = Names.Contains(Pair.Key) ? *It : nullptr;
		}

		if (!ColumnProp)
		{
			ImportProblems.Add(FString::Printf(TEXT("Property '%s' on row '%s' cannot be found in struct '%s'."), *PropName.ToString(), *RowName.ToString(), *Struct->GetName()));
		}
	}
}

/***********************************
 * FPsDataTableDeserializer
 ***********************************/

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

bool FPsDataTableDeserializer::ReadValue(int64& OutValue)
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

TSharedPtr<FJsonObject> FPsDataTableDeserializer::CreateJsonFromTable(UDataTable* DataTable, const FString& PropertyName)
{
	TSharedPtr<FJsonObject> RootJsonObject = MakeShared<FJsonObject>();

	const UScriptStruct* Struct = DataTable->GetRowStruct();
	check(Struct);
	const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
	if (RowMap.Num() > 0)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		for (auto& Pair : RowMap)
		{
			JsonObject->SetObjectField(Pair.Key.ToString().ToLower(), FPsDataStructDeserializer::CreateJsonFromStruct(Struct, Pair.Value));
		}
		RootJsonObject->SetObjectField(PropertyName, JsonObject);
	}

	return RootJsonObject;
}

TArray<TSharedPtr<FJsonValue>> FPsDataTableDeserializer::CreateJsonFromTable_Export(UDataTable* DataTable)
{
	const FString KeyField = DataTable->ImportKeyField.IsEmpty() ? TEXT("Name") : DataTable->ImportKeyField;

	TArray<TSharedPtr<FJsonValue>> Result;

	const UScriptStruct* Struct = DataTable->GetRowStruct();
	check(Struct);

	const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
	for (auto& Pair : RowMap)
	{
		auto JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetField(KeyField, MakeShared<FJsonValueString>(Pair.Key.ToString().ToLower()));

		auto StructJsonObject = FPsDataStructDeserializer::CreateJsonFromStruct_Export(Struct, Pair.Value);
		for (auto& FieldPair : StructJsonObject->Values)
		{
			JsonObject->SetField(FieldPair.Key, FieldPair.Value);
		}

		Result.Add(MakeShared<FJsonValueObject>(JsonObject));
	}

	return Result;
}
