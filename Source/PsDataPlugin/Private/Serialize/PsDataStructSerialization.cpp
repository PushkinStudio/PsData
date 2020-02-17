// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataStructSerialization.h"

#include "PsData.h"
#include "PsDataCore.h"

#include "Internationalization/Regex.h"
#include "JsonObjectConverter.h"

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
	TSharedPtr<FJsonValue> JsonValue = StructSerialize(Struct, Value);
	check(JsonValue->Type == EJson::Object);

	/*
	 FString JsonString;
	 auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	 FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer);
	 UE_LOG(LogData, Warning, TEXT("Json: %s"), *JsonString);
	 */

	return JsonValue->AsObject();
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::PropertySerialize(UProperty* Property, const void* Value)
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

	if (USoftObjectProperty* SoftObjectProperty = Cast<USoftObjectProperty>(Property))
	{
		const FSoftObjectPtr& SoftObjectPtr = SoftObjectProperty->GetPropertyValue(Value);
		const FSoftObjectPath& SoftObjectPath = SoftObjectPtr.ToSoftObjectPath();
		TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
		JsonObject->Values.Add("AssetPathName", TSharedPtr<FJsonValue>(new FJsonValueString(SoftObjectPath.GetAssetPathName().ToString())));
		JsonObject->Values.Add("SubPathString", TSharedPtr<FJsonValue>(new FJsonValueString(SoftObjectPath.GetSubPathString())));
		return TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObject));
	}

	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::StructPropertySerialize(UStructProperty* StructProperty, const void* Value)
{
	if (StructProperty->Struct)
	{
		return StructSerialize(StructProperty->Struct, Value);
	}
	return TSharedPtr<FJsonValue>(nullptr);
}

TSharedPtr<FJsonValue> FPsDataStructDeserializer::StructSerialize(const UStruct* Struct, const void* Value)
{
	FJsonObjectConverter::CustomExportCallback CustomCallback;
	CustomCallback.BindLambda([](UProperty* Property, const void* Ptr) {
		return PropertySerialize(Property, Ptr);
	});

	TSharedPtr<FJsonObject> RawJsonObject(new FJsonObject());
	FJsonObjectConverter::UStructToJsonAttributes(Struct, Value, RawJsonObject->Values, 0, 0, &CustomCallback);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	for (auto& Pair : RawJsonObject->Values)
	{
		JsonObject->Values.Add(GetNormalizedKey(Pair.Key), Pair.Value);
	}

	return TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObject));
}

FString FPsDataStructDeserializer::GetNormalizedKey(const FString& Key)
{
	static const FRegexPattern PropertyPattern(TEXT("(.+)_\\d+_[A-Z0-9]+"));
	FRegexMatcher PropertyMatcher(PropertyPattern, Key);
	if (PropertyMatcher.FindNext())
	{
		return PropertyMatcher.GetCaptureGroup(1);
	}
	else
	{
		return Key;
	}
}
