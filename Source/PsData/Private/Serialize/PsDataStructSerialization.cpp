// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Serialize/PsDataStructSerialization.h"

#include "PsData.h"
#include "PsDataCore.h"

#include "Internationalization/Regex.h"
#include "JsonObjectConverter.h"
#include "UObject/TextProperty.h"

/***********************************
 * FPsDataStructSerializer
 ***********************************/

void FPsDataStructSerializer::WriteKey(const FString& Key)
{
	JsonSerializer.WriteKey(Key);
}

void FPsDataStructSerializer::WriteArray()
{
	JsonSerializer.WriteArray();
}

void FPsDataStructSerializer::WriteObject()
{
	JsonSerializer.WriteObject();
}

void FPsDataStructSerializer::WriteValue(int32 Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(int64 Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(uint8 Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(float Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(bool Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(const FString& Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(const FName& Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::WriteValue(const UPsData* Value)
{
	JsonSerializer.WriteValue(Value);
}

void FPsDataStructSerializer::PopKey(const FString& Key)
{
	JsonSerializer.PopKey(Key);
}

void FPsDataStructSerializer::PopArray()
{
	JsonSerializer.PopArray();
}

void FPsDataStructSerializer::PopObject()
{
	JsonSerializer.PopObject();
}

/***********************************
 * Struct deserialize
 ***********************************/

uint8* FPsDataStructSerializer::CreateStructFromJson(UStruct* Struct, const TSharedRef<FJsonObject>& JsonObject, TMap<FString, FString>& KeyMap)
{
	/*
	FString JsonString;
	auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	UE_LOG(LogData, Warning, TEXT("Json: %s"), *JsonString);
	*/

	uint8* Dest = static_cast<uint8*>(FMemory::Malloc(Struct->GetStructureSize()));
	StructDeserialize(Struct, Dest, JsonObject, KeyMap);
	return Dest;
}

void FPsDataStructSerializer::PropertyDeserialize(FProperty* Property, uint8* OutDest, const TSharedRef<FJsonValue>& JsonValue, TMap<FString, FString>& KeyMap)
{
	if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		if (JsonValue->Type == EJson::Object)
		{
			const auto JsonObject = JsonValue->AsObject();
			if (JsonObject->HasField("TableId") && JsonObject->HasField("Key"))
			{
				const auto TableIdValue = JsonObject->GetStringField("TableId");
				const auto KeyValue = JsonObject->GetStringField("Key");
				TextProperty->SetPropertyValue(OutDest, FText::FromStringTable(*TableIdValue, KeyValue));
				return;
			}
			else if (JsonObject->HasField("Empty") && JsonObject->GetBoolField("Empty"))
			{
				TextProperty->SetPropertyValue(OutDest, FText::GetEmpty());
				return;
			}
		}
		else if (JsonValue->Type == EJson::String)
		{
			TextProperty->SetPropertyValue(OutDest, FText::FromString(JsonValue->AsString()));
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Property->Owner.GetName(), *TextProperty->GetCPPType());
	}
	else if (FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
	{
		if (JsonValue->Type == EJson::Object)
		{
			const auto JsonObject = JsonValue->AsObject();
			const auto AssetPathNameValue = JsonObject->GetStringField("AssetPathName");
			const auto SubPathStringValue = JsonObject->GetStringField("SubPathString");
			SoftObjectProperty->SetPropertyValue(OutDest, FSoftObjectPtr(FSoftObjectPath(*AssetPathNameValue, SubPathStringValue)));
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Property->Owner.GetName(), *SoftObjectProperty->GetCPPType(nullptr, 0));
	}
	else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		const auto Enum = EnumProperty->GetEnum();
		if (JsonValue->Type == EJson::String)
		{
			const auto Index = Enum->GetIndexByNameString(JsonValue->AsString());
			if (Index != INDEX_NONE)
			{
				const auto Value = Enum->GetValueByIndex(Index);
				EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutDest, Value);
				return;
			}
		}
		else if (JsonValue->Type == EJson::Number)
		{
			int64 Value = 0;
			if (JsonValue->TryGetNumber(Value))
			{
				EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutDest, Value);
				return;
			}
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Property->Owner.GetName(), *EnumProperty->GetCPPType(nullptr, 0));
	}
	else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		if (JsonValue->Type == EJson::Object)
		{
			StructDeserialize(StructProperty->Struct, OutDest, JsonValue->AsObject().ToSharedRef(), KeyMap);
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Property->Owner.GetName(), *Property->GetName(), *StructProperty->GetCPPType(nullptr, 0));
	}

	FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, OutDest, 0, 0);
}

void FPsDataStructSerializer::StructDeserialize(UStruct* Struct, uint8* OutDest, const TSharedRef<FJsonObject>& JsonObject, TMap<FString, FString>& KeyMap)
{
	for (FProperty* Property = Struct->PropertyLink; Property != nullptr; Property = Property->PropertyLinkNext)
	{
		auto JsonValue = FindJsonValueByProperty(Property, JsonObject, KeyMap);
		auto ValueDest = OutDest + Property->GetOffset_ForInternal();
		Property->InitializeValue(ValueDest);

		if (JsonValue.IsValid())
		{
			if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
			{
				if (JsonValue->Type == EJson::Array)
				{
					auto JsonArray = JsonValue->AsArray();
					if (JsonArray.Num() > 0)
					{
						FScriptArrayHelper ScriptArrayHelper(ArrayProperty, ValueDest);
						ScriptArrayHelper.AddValues(JsonArray.Num());
						for (int32 i = 0; i < JsonArray.Num(); ++i)
						{
							PropertyDeserialize(ArrayProperty->Inner, ScriptArrayHelper.GetRawPtr(i), JsonArray[i].ToSharedRef(), KeyMap);
						}
					}
				}
				else
				{
					UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Property->Owner.GetName(), *Property->GetName(), *ArrayProperty->GetCPPType(nullptr, 0));
				}
			}
			else if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
			{
				if (JsonValue->Type == EJson::Object)
				{
					auto JsonMap = JsonValue->AsObject();
					FScriptMapHelper ScriptMapHelper(MapProperty, ValueDest);
					for (auto& Pair : JsonMap->Values)
					{
						const auto Index = ScriptMapHelper.AddDefaultValue_Invalid_NeedsRehash();
						PropertyDeserialize(MapProperty->KeyProp, ScriptMapHelper.GetKeyPtr(Index), MakeShared<FJsonValueString>(Pair.Key), KeyMap);
						PropertyDeserialize(MapProperty->ValueProp, ScriptMapHelper.GetValuePtr(Index), Pair.Value.ToSharedRef(), KeyMap);
					}
				}
				else
				{
					UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Property->Owner.GetName(), *Property->GetName(), *MapProperty->GetCPPType(nullptr, 0));
				}
			}
			else
			{
				PropertyDeserialize(Property, ValueDest, JsonValue.ToSharedRef(), KeyMap);
			}
		}
	}
}

TSharedPtr<FJsonValue> FPsDataStructSerializer::FindJsonValueByProperty(const FProperty* Property, const TSharedRef<FJsonObject>& JsonObject, TMap<FString, FString>& KeyMap)
{
	const auto& PropertyName = FPsDataStructDeserializer::GetNormalizedKey(Property->GetName(), KeyMap);
	if (auto Find = JsonObject->Values.Find(PropertyName))
	{
		return *Find;
	}

	return nullptr;
}

/***********************************
 * FPsDataStructDeserializer
 ***********************************/

bool FPsDataStructDeserializer::ReadKey(FString& OutKey)
{
	return JsonDeserializer.ReadKey(OutKey);
}

bool FPsDataStructDeserializer::ReadIndex()
{
	return JsonDeserializer.ReadIndex();
}

bool FPsDataStructDeserializer::ReadArray()
{
	return JsonDeserializer.ReadArray();
}

bool FPsDataStructDeserializer::ReadObject()
{
	return JsonDeserializer.ReadObject();
}

bool FPsDataStructDeserializer::ReadValue(int32& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(int64& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(uint8& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(float& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(bool& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(FString& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(FName& OutValue)
{
	return JsonDeserializer.ReadValue(OutValue);
}

bool FPsDataStructDeserializer::ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator)
{
	return JsonDeserializer.ReadValue(OutValue, Allocator);
}

void FPsDataStructDeserializer::PopKey(const FString& Key)
{
	return JsonDeserializer.PopKey(Key);
}

void FPsDataStructDeserializer::PopIndex()
{
	return JsonDeserializer.PopIndex();
}

void FPsDataStructDeserializer::PopArray()
{
	return JsonDeserializer.PopArray();
}

void FPsDataStructDeserializer::PopObject()
{
	return JsonDeserializer.PopObject();
}

/***********************************
 * Struct serialize
 ***********************************/

TSharedPtr<FJsonObject> FPsDataStructDeserializer::CreateJsonFromStruct(const UStruct* Struct, const void* Value, TMap<FString, FString>& KeyMap)
{
	const TSharedPtr<FJsonValue> JsonValue = StructSerialize(Struct, Value, KeyMap);
	check(JsonValue->Type == EJson::Object);

	/*
	FString JsonString;
	auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonValue->AsObject().ToSharedRef(), Writer);
	UE_LOG(LogData, Warning, TEXT("Json: %s"), *JsonString);
	*/

	return JsonValue->AsObject();
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::PropertySerialize(FProperty* Property, const void* Value, TMap<FString, FString>& KeyMap)
{
	if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		return StructPropertySerialize(StructProperty, Value, KeyMap);
	}

	if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		const FText& Text = TextProperty->GetPropertyValue(Value);
		if (Text.IsFromStringTable())
		{
			FName TableId;
			FString Key;
			FTextInspector::GetTableIdAndKey(Text, TableId, Key);
			TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
			JsonObject->Values.Add("TableId", MakeShared<FJsonValueString>(TableId.ToString()));
			JsonObject->Values.Add("Key", MakeShared<FJsonValueString>(Key));
			return MakeShared<FJsonValueObject>(JsonObject);
		}
		else if (Text.IsEmpty())
		{
			TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
			JsonObject->Values.Add("Empty", MakeShared<FJsonValueBoolean>(true));
			return MakeShared<FJsonValueObject>(JsonObject);
		}
		else
		{
			return MakeShared<FJsonValueString>(Text.ToString());
		}
	}

	if (FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
	{
		const FSoftObjectPtr& SoftObjectPtr = SoftObjectProperty->GetPropertyValue(Value);
		const FSoftObjectPath& SoftObjectPath = SoftObjectPtr.ToSoftObjectPath();
		TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
		JsonObject->Values.Add("AssetPathName", MakeShared<FJsonValueString>(SoftObjectPath.GetAssetPathName().ToString()));
		JsonObject->Values.Add("SubPathString", MakeShared<FJsonValueString>(SoftObjectPath.GetSubPathString()));
		return MakeShared<FJsonValueObject>(JsonObject);
	}

	if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		const auto Enum = EnumProperty->GetEnum();
		const auto EnumValue = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Value);
		const auto EnumIndex = Enum->GetIndexByValue(EnumValue);
		if (EnumIndex == INDEX_NONE)
		{
			return MakeShared<FJsonValueNumberString>(FString::Printf(TEXT("%lld"), EnumValue));
		}
		else
		{
			return MakeShared<FJsonValueString>(Enum->GetNameStringByIndex(EnumIndex));
		}
	}

	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::StructPropertySerialize(FStructProperty* StructProperty, const void* Value, TMap<FString, FString>& KeyMap)
{
	if (StructProperty->Struct)
	{
		return StructSerialize(StructProperty->Struct, Value, KeyMap);
	}

	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::StructSerialize(const UStruct* Struct, const void* Value, TMap<FString, FString>& KeyMap)
{
	FJsonObjectConverter::CustomExportCallback CustomCallback;
	CustomCallback.BindLambda([&KeyMap](FProperty* Property, const void* Ptr) {
		return PropertySerialize(Property, Ptr, KeyMap);
	});

	TSharedPtr<FJsonObject> RawJsonObject(new FJsonObject());
	FJsonObjectConverter::UStructToJsonAttributes(Struct, Value, RawJsonObject->Values, 0, 0, &CustomCallback);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	for (auto& Pair : RawJsonObject->Values)
	{
		JsonObject->Values.Add(GetNormalizedKey(Pair.Key, KeyMap), Pair.Value);
	}

	return MakeShared<FJsonValueObject>(JsonObject);
}

const FString& FPsDataStructDeserializer::GetNormalizedKey(const FString& Key, TMap<FString, FString>& KeyMap)
{
	if (const auto KeyPtr = KeyMap.Find(Key))
	{
		return *KeyPtr;
	}

	static const FRegexPattern PropertyPattern(TEXT("(.+)_\\d+_[A-Z0-9]+"));
	FRegexMatcher PropertyMatcher(PropertyPattern, Key);
	if (PropertyMatcher.FindNext())
	{
		return KeyMap.Add(Key, PropertyMatcher.GetCaptureGroup(1));
	}
	else
	{
		return KeyMap.Add(Key, Key);
	}
}
