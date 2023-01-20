// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

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

uint8* FPsDataStructSerializer::CreateStructFromJson(const UStruct* Struct, const TSharedRef<FJsonObject>& JsonObject, bool bInitialize)
{
	uint8* Dest = static_cast<uint8*>(FMemory::Malloc(Struct->GetStructureSize()));
	StructDeserialize(Struct, Dest, JsonObject, bInitialize);
	return Dest;
}

uint8* FPsDataStructSerializer::CreateStructFromJson_Import(const UStruct* Struct, const TSharedRef<FJsonObject>& JsonObject, TArray<FString>& ImportProblems)
{
	return CreateStructFromJson(Struct, JsonObject, true);
}

void FPsDataStructSerializer::PropertyDeserialize(FProperty* Property, uint8* OutDest, const TSharedRef<FJsonValue>& JsonValue)
{
	if (const auto TextProperty = CastField<FTextProperty>(Property))
	{
		if (JsonValue->Type == EJson::String)
		{
			FText Text = FText::GetEmpty();
			FTextStringHelper::ReadFromBuffer(*JsonValue->AsString(), Text);
			TextProperty->SetPropertyValue(OutDest, Text);
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as \"%s\""), *Property->GetAuthoredName(), *TextProperty->GetCPPType());
	}
	else if (const auto SoftClassProperty = CastField<FSoftClassProperty>(Property))
	{
		if (JsonValue->Type == EJson::String)
		{
			SoftClassProperty->SetPropertyValue(OutDest, FSoftObjectPtr(FSoftClassPath(JsonValue->AsString())));
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as \"%s\""), *Property->GetAuthoredName(), *SoftClassProperty->GetCPPType(nullptr, 0));
	}
	else if (const auto SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
	{
		if (JsonValue->Type == EJson::String)
		{
			SoftObjectProperty->SetPropertyValue(OutDest, FSoftObjectPtr(FSoftObjectPath(JsonValue->AsString())));
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as \"%s\""), *Property->GetAuthoredName(), *SoftObjectProperty->GetCPPType(nullptr, 0));
	}
	else if (const auto StructProperty = CastField<FStructProperty>(Property))
	{
		if (JsonValue->Type == EJson::Object)
		{
			StructDeserialize(StructProperty->Struct, OutDest, JsonValue->AsObject().ToSharedRef(), true);
			return;
		}

		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as \"%s\""), *Property->GetAuthoredName(), *StructProperty->GetCPPType(nullptr, 0));
	}

	FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, OutDest, 0, 0);
}

void FPsDataStructSerializer::StructDeserialize(const UStruct* Struct, uint8* OutDest, const TSharedRef<FJsonObject>& JsonObject, bool bInitialize)
{
	if (bInitialize)
	{
		Struct->InitializeStruct(OutDest);
	}

	for (TFieldIterator<FProperty> It(Struct); It; ++It)
	{
		FProperty* Property = *It;
		auto JsonValue = FindJsonValueByProperty(Property, JsonObject);
		auto ValueDest = Property->ContainerPtrToValuePtr<uint8>(OutDest);
		Property->InitializeValue(ValueDest);

		if (JsonValue.IsValid())
		{
			if (const auto ArrayProperty = CastField<FArrayProperty>(Property))
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
							PropertyDeserialize(ArrayProperty->Inner, ScriptArrayHelper.GetRawPtr(i), JsonArray[i].ToSharedRef());
						}
					}
				}
				else
				{
					UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as \"%s\""), *Property->GetAuthoredName(), *ArrayProperty->GetCPPType(nullptr, 0));
				}
			}
			else if (const auto MapProperty = CastField<FMapProperty>(Property))
			{
				if (JsonValue->Type == EJson::Object)
				{
					auto JsonMap = JsonValue->AsObject();
					FScriptMapHelper ScriptMapHelper(MapProperty, ValueDest);
					for (auto& Pair : JsonMap->Values)
					{
						const auto Index = ScriptMapHelper.AddDefaultValue_Invalid_NeedsRehash();
						PropertyDeserialize(MapProperty->KeyProp, ScriptMapHelper.GetKeyPtr(Index), MakeShared<FJsonValueString>(Pair.Key));
						PropertyDeserialize(MapProperty->ValueProp, ScriptMapHelper.GetValuePtr(Index), Pair.Value.ToSharedRef());
					}
				}
				else
				{
					UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as \"%s\""), *Property->GetAuthoredName(), *MapProperty->GetCPPType(nullptr, 0));
				}
			}
			else
			{
				PropertyDeserialize(Property, ValueDest, JsonValue.ToSharedRef());
			}
		}
	}
}

TSharedPtr<FJsonValue> FPsDataStructSerializer::FindJsonValueByProperty(const FProperty* Property, const TSharedRef<FJsonObject>& JsonObject)
{
	if (auto Find = JsonObject->Values.Find(Property->GetAuthoredName()))
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

TSharedPtr<FJsonObject> FPsDataStructDeserializer::CreateJsonFromStruct(const UStruct* Struct, const void* Value)
{
	const TSharedPtr<FJsonValue> JsonValue = StructSerialize(Struct, Value);
	check(JsonValue->Type == EJson::Object);
	return JsonValue->AsObject();
}

TSharedPtr<FJsonObject> FPsDataStructDeserializer::CreateJsonFromStruct_Export(const UStruct* Struct, const void* Value)
{
	return CreateJsonFromStruct(Struct, Value);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::PropertySerialize(FProperty* Property, const void* Value)
{
	if (const auto StructProperty = CastField<FStructProperty>(Property))
	{
		return StructPropertySerialize(StructProperty, Value);
	}

	if (const auto TextProperty = CastField<FTextProperty>(Property))
	{
		const FText& TextValue = TextProperty->GetPropertyValue(Value);
		FString ValueStr;
		FTextStringHelper::WriteToBuffer(ValueStr, TextValue);
		return MakeShared<FJsonValueString>(ValueStr);
	}

	if (const auto SoftClassProperty = CastField<FSoftClassProperty>(Property))
	{
		const auto& SoftClassPtr = SoftClassProperty->GetPropertyValue(Value);
		return MakeShared<FJsonValueString>(SoftClassPtr.ToString());
	}

	if (const auto SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
	{
		const auto& SoftObjectPtr = SoftObjectProperty->GetPropertyValue(Value);
		return MakeShared<FJsonValueString>(SoftObjectPtr.ToString());
	}

	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::StructPropertySerialize(FStructProperty* StructProperty, const void* Value)
{
	if (StructProperty->Struct)
	{
		const auto CppStructOps = StructProperty->Struct->GetCppStructOps();
		if (CppStructOps && CppStructOps->HasExportTextItem() && CppStructOps->HasImportTextItem())
		{
			FString ResultString;
			CppStructOps->ExportTextItem(ResultString, Value, nullptr, nullptr, PPF_None, nullptr);
			return MakeShared<FJsonValueString>(ResultString);
		}

		return StructSerialize(StructProperty->Struct, Value);
	}

	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::StructSerialize(const UStruct* Struct, const void* Value)
{
	FJsonObjectConverter::CustomExportCallback CustomCallback;
	CustomCallback.BindLambda([](FProperty* Property, const void* Ptr) {
		return PropertySerialize(Property, Ptr);
	});

	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	for (TFieldIterator<FProperty> It(Struct); It; ++It)
	{
		FProperty* Property = *It;
		const void* PropertyValue = Property->ContainerPtrToValuePtr<uint8>(Value);

		TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(Property, PropertyValue, 0, 0, &CustomCallback);
		if (JsonValue.IsValid())
		{
			JsonObject->SetField(Property->GetAuthoredName(), JsonValue);
		}
		else
		{
			UE_LOG(LogData, Warning, TEXT("Unsupported type \"%s\" for \"%s\""), *Property->GetCPPType(), *Property->GetAuthoredName());
		}
	}

	return MakeShared<FJsonValueObject>(JsonObject);
}
