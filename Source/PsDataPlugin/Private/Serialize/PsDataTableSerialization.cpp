// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataTableSerialization.h"

#include "PsData.h"
#include "PsDataCore.h"
#include "Serialize/PsDataJsonSerialization.h"

#include "JsonObjectConverter.h"

FPsDataTableDeserializer::FPsDataTableDeserializer(UDataTable* DataTable, const FString& InPropertyName)
	: FPsDataDeserializer()
	, DataTablePtr(DataTable)
	, PropertyName(InPropertyName)
	, PropertyPattern(TEXT("(.+)_\\d+_[A-Z0-9]+"))
	, PropertyMap()
{
	check(DataTablePtr.IsValid());
}

bool FPsDataTableDeserializer::Has(const TSharedPtr<const FDataField>& Field)
{
	return (PropertyName == Field->Name);
}

/***********************************
 * int32
 ***********************************/

int32 FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, int32 Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return 0;
}

TArray<int32> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TArray<int32>();
}

TMap<FString, int32> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TMap<FString, int32>();
}

/***********************************
 * uint8
 ***********************************/

uint8 FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, uint8 Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return 0;
}

TArray<uint8> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TArray<uint8>();
}

TMap<FString, uint8> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TMap<FString, uint8>();
}

/***********************************
 * float
 ***********************************/

float FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, float Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return 0.f;
}

TArray<float> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<float>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TArray<float>();
}

TMap<FString, float> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TMap<FString, float>();
}

/***********************************
 * string
 ***********************************/

FString FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return FString();
}

TArray<FString> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TArray<FString>();
}

TMap<FString, FString> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TMap<FString, FString>();
}

/***********************************
 * bool
 ***********************************/

bool FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, bool Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return false;
}

TArray<bool> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TArray<bool>();
}

TMap<FString, bool> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TMap<FString, bool>();
}

/***********************************
 * Data
 ***********************************/

UPsData* FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, UPsData* Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return nullptr;
}

TArray<UPsData*> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TMap<FString, T*>"));
	return TArray<UPsData*>();
}

TMap<FString, UPsData*> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data, UClass* DataClass)
{
	if (DataTablePtr.IsValid())
	{
		const UScriptStruct* Struct = DataTablePtr->GetRowStruct();
		CompareTableAndClass(Struct, DataClass);

		const TMap<FName, uint8*>& RowMap = DataTablePtr->GetRowMap();
		if (RowMap.Num() > 0 && Struct != nullptr)
		{
			TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
			for (auto& Row : RowMap)
			{
				TSharedPtr<FJsonValue> JsonValue = StructSerialize(Struct, (void*)Row.Value);
				if (JsonValue->Type == EJson::Object)
				{
					JsonObject->SetObjectField(Row.Key.ToString(), JsonValue->AsObject());
				}
			}
			TSharedPtr<FJsonObject> RootJsonObject(new FJsonObject());
			RootJsonObject->SetObjectField(Field->Name, JsonObject);

			/*
			FString JsonString;
			auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
			FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);
			UE_LOG(LogData, Warning, TEXT("Json: %s"), *JsonString);
			*/

			TMap<FString, UPsData*>* Map = nullptr;
			if (FDataReflectionTools::GetByField(Instance, Field, Map))
			{
				FPsDataJsonDeserializer JsonDeserializer(RootJsonObject);
				return JsonDeserializer.Deserialize(Instance, Field, *Map, DataClass);
			}
		}
	}
	else
	{
		UE_LOG(LogData, Error, TEXT("Can't deserialize table \"%s\""), *DataTablePtr->GetPathName());
	}

	return TMap<FString, UPsData*>();
}

/***********************************
* Custom
***********************************/

TSharedPtr<FJsonValue> FPsDataTableDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data, UClass* DataClass)
{
	UE_LOG(LogData, Error, TEXT("For FTableDataSerializer property type must be TSharedPtr<FJsonValue>"));
	return TSharedPtr<FJsonValue>(nullptr);
}

/***********************************
* Struct serialize
***********************************/

void FPsDataTableDeserializer::CompareTableAndClass(const UStruct* Struct, UClass* DataClass)
{
#if WITH_EDITORONLY_DATA
	auto& Fields = FDataReflection::GetFields(DataClass);
	TSet<FString> PropertySet;
	for (UProperty* Property = Struct->PropertyLink; Property; Property = Property->PropertyLinkNext)
	{
		const FString Name = Property->GetName();
		FRegexMatcher PropertyMatcher(PropertyPattern, Name);
		if (PropertyMatcher.FindNext())
		{
			PropertySet.Add(PropertyMatcher.GetCaptureGroup(1));
		}
		else
		{
			PropertySet.Add(Name);
		}
	}

	for (FString& Name : PropertySet)
	{
		if (!Fields.Contains(Name))
		{
			UE_LOG(LogData, Error, TEXT("Can't find property \"%s::%s\", but it contains in the table \"%s\""), *DataClass->GetName(), *Name, *DataTablePtr->GetPathName());
		}
	}
	for (auto& Pair : Fields)
	{
		if (!PropertySet.Contains(Pair.Key))
		{
			UE_LOG(LogData, Error, TEXT("Can't find property \"%s\" in \"%s\", but it contains in the class \"%s\""), *Pair.Key, *DataTablePtr->GetPathName(), *DataClass->GetName());
		}
	}
#endif //WITH_EDITORONLY_DATA
}

TMap<FString, FString>& FPsDataTableDeserializer::UpdatePropertyMap(const UStruct* Struct, TMap<FString, TSharedPtr<FJsonValue>>& Values)
{
	auto& Map = PropertyMap.FindOrAdd(Struct->GetUniqueID());
	for (auto& Pair : Values)
	{
		FRegexMatcher PropertyMatcher(PropertyPattern, Pair.Key);
		if (PropertyMatcher.FindNext())
		{
			Map.Add(Pair.Key, PropertyMatcher.GetCaptureGroup(1));
		}
		else
		{
			Map.Add(Pair.Key, Pair.Key);
		}
	}
	return Map;
}

TSharedPtr<FJsonValue> FPsDataTableDeserializer::PropertySerialize(UProperty* Property, const void* Value)
{
	if (UStructProperty* StructProperty = Cast<UStructProperty>(Property))
	{
		return StructPropertySerialize(StructProperty, Value);
	}

	if (UTextProperty* TextProperty = Cast<UTextProperty>(Property))
	{
		const FText& Text = TextProperty->GetPropertyValue(Value);
		if (Text.IsFromStringTable())
		{
			FName TableId;
			FString Key;
			FTextInspector::GetTableIdAndKey(Text, TableId, Key);
			TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
			JsonObject->Values.Add("TableId", TSharedPtr<FJsonValue>(new FJsonValueString(TableId.ToString())));
			JsonObject->Values.Add("Key", TSharedPtr<FJsonValue>(new FJsonValueString(Key)));
			return TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObject));
		}
		else
		{
			return TSharedPtr<FJsonValue>(new FJsonValueString(Text.ToString()));
		}
	}

	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataTableDeserializer::StructPropertySerialize(UStructProperty* StructProperty, const void* Value)
{
	if (StructProperty->Struct)
	{
		return StructSerialize(StructProperty->Struct, Value);
	}
	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataTableDeserializer::StructSerialize(const UStruct* Struct, const void* Value)
{
	FJsonObjectConverter::CustomExportCallback CustomCallback;
	CustomCallback.BindLambda([this](UProperty* Property, const void* Ptr) {
		return PropertySerialize(Property, Ptr);
	});

	TSharedPtr<FJsonObject> RawJsonObject(new FJsonObject());
	FJsonObjectConverter::UStructToJsonAttributes(Struct, Value, RawJsonObject->Values, 0, 0, &CustomCallback);

	auto& Map = UpdatePropertyMap(Struct, RawJsonObject->Values);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	for (auto& Pair : RawJsonObject->Values)
	{
		auto Find = Map.Find(Pair.Key);
		if (Find)
		{
			JsonObject->Values.Add(*Find, Pair.Value);
		}
	}

	return TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObject));
}
