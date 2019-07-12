// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataJsonSerialization.h"

#include "PsData.h"

/***********************************
 * FJsonDataSerializer
 ***********************************/

FPsDataJsonSerializer::FPsDataJsonSerializer(TSharedPtr<FJsonObject> InJson)
	: FPsDataSerializer(false)
	, RootJson(InJson)
{
	check(InJson.IsValid());
	InJson->Values.Empty();

	Values.Reserve(10);
	Values.Add(MakeShareable(new FJsonValueObject(InJson)));
	Keys.Reserve(10);
}

FPsDataJsonSerializer::FPsDataJsonSerializer()
	: FPsDataJsonSerializer(TSharedPtr<FJsonObject>(new FJsonObject()))
{
}

TSharedPtr<FJsonObject>& FPsDataJsonSerializer::GetJson()
{
	check(Values.Num() == 1);
	check(Keys.Num() == 0);
	return RootJson;
}

void FPsDataJsonSerializer::WriteJsonValue(TSharedPtr<FJsonValue> Value)
{
	check(Values.Num() > 0);
	TSharedPtr<FJsonValue> Parent = Values.Last();
	if (Parent->Type == EJson::Array)
	{
		const TArray<TSharedPtr<FJsonValue>>* ArrayPtr = nullptr;
		Parent->TryGetArray(ArrayPtr);
		TArray<TSharedPtr<FJsonValue>>& Array = *const_cast<TArray<TSharedPtr<FJsonValue>>*>(ArrayPtr); // :(
		Array.Add(Value);
	}
	else if (Parent->Type == EJson::Object)
	{
		check(Keys.Num() > 0);
		Parent->AsObject()->Values.Add(Keys.Last(), Value);
	}

	if (Value->Type == EJson::Array || Value->Type == EJson::Object)
	{
		Values.Add(Value);
	}
}

void FPsDataJsonSerializer::WriteKey(const FString& Key)
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Object);
	Keys.Add(Key);
}

void FPsDataJsonSerializer::WriteArray()
{
	WriteJsonValue(MakeShareable(new FJsonValueArray(TArray<TSharedPtr<FJsonValue>>())));
}

void FPsDataJsonSerializer::WriteObject()
{
	WriteJsonValue(MakeShareable(new FJsonValueObject(MakeShareable(new FJsonObject()))));
}

void FPsDataJsonSerializer::WriteValue(int32 Value)
{
	WriteJsonValue(MakeShareable(new FJsonValueNumber(Value)));
}

void FPsDataJsonSerializer::WriteValue(uint8 Value)
{
	WriteJsonValue(MakeShareable(new FJsonValueNumber(Value)));
}

void FPsDataJsonSerializer::WriteValue(float Value)
{
	WriteJsonValue(MakeShareable(new FJsonValueNumber(Value)));
}

void FPsDataJsonSerializer::WriteValue(bool Value)
{
	WriteJsonValue(MakeShareable(new FJsonValueBoolean(Value)));
}

void FPsDataJsonSerializer::WriteValue(const FString& Value)
{
	WriteJsonValue(MakeShareable(new FJsonValueString(Value)));
}

void FPsDataJsonSerializer::WriteValue(const FName& Value)
{
	WriteJsonValue(MakeShareable(new FJsonValueString(Value.ToString())));
}

void FPsDataJsonSerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		WriteJsonValue(MakeShareable(new FJsonValueNull()));
	}
	else
	{
		WriteObject();
		Value->DataSerialize(this);
		PopObject();
	}
}

void FPsDataJsonSerializer::PopKey(const FString& Key)
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Object);
	check(Keys.Num() > 0 && Keys.Last() == Key);
	Keys.Pop(false);
}

void FPsDataJsonSerializer::PopArray()
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Array);
	Values.Pop(false);
}

void FPsDataJsonSerializer::PopObject()
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Object);
	Values.Pop(false);
}

/***********************************
 * FPsDataJsonDeserializer
 ***********************************/

FPsDataJsonDeserializer::FPsDataJsonDeserializer(TSharedPtr<FJsonObject> InJson)
	: FPsDataDeserializer()
	, RootJson(InJson)
{
	check(InJson.IsValid());

	Values.Reserve(10);
	Values.Add(MakeShareable(new FJsonValueObject(InJson)));
	KeysIterator.Reserve(10);

#if WITH_EDITORONLY_DATA
	Used.Reserve(100);
#endif
}

TSharedPtr<FJsonObject>& FPsDataJsonDeserializer::GetJson()
{
	check(Values.Num() == 1);
	return RootJson;
}

TSharedPtr<FJsonValue> FPsDataJsonDeserializer::ReadJsonValue()
{
	check(Values.Num() > 0);
	TSharedPtr<FJsonValue> Parent = Values.Last();
	TSharedPtr<FJsonValue> Value;
	if (Parent->Type == EJson::Array)
	{
		auto& Iterator = IndicesIterator.FindChecked(Parent);
		Value = *Iterator;
	}
	else if (Parent->Type == EJson::Object)
	{
		auto& Iterator = KeysIterator.FindChecked(Parent);
		Value = Iterator.Value();
	}
	check(Value.IsValid());
	return Value;
}

bool FPsDataJsonDeserializer::ReadKey(FString& OutKey)
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Object);
	TSharedPtr<FJsonValue> JsonValue = Values.Last();

#if WITH_EDITORONLY_DATA
	check(!Used.Contains(JsonValue));
#endif

	if (!KeysIterator.Contains(JsonValue))
	{
		TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
		KeysIterator.Add(JsonValue, JsonObject->Values.CreateIterator());
	}

	auto& Iterator = KeysIterator.FindChecked(JsonValue);
	if (Iterator)
	{
		OutKey = Iterator.Key();
		return true;
	}
	else
	{
		KeysIterator.Remove(JsonValue);
#if WITH_EDITORONLY_DATA
		Used.Add(JsonValue);
#endif
		return false;
	}
}

bool FPsDataJsonDeserializer::ReadIndex()
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Array);
	TSharedPtr<FJsonValue> JsonValue = Values.Last();

#if WITH_EDITORONLY_DATA
	check(!Used.Contains(JsonValue));
#endif

	if (!IndicesIterator.Contains(JsonValue))
	{
		const TArray<TSharedPtr<FJsonValue>>* ArrayPtr = nullptr;
		JsonValue->TryGetArray(ArrayPtr);
		TArray<TSharedPtr<FJsonValue>>& Array = *const_cast<TArray<TSharedPtr<FJsonValue>>*>(ArrayPtr); // :(
		IndicesIterator.Add(JsonValue, Array.CreateIterator());
	}

	auto& Iterator = IndicesIterator.FindChecked(JsonValue);
	if (Iterator)
	{
		return true;
	}
	else
	{
		IndicesIterator.Remove(JsonValue);
#if WITH_EDITORONLY_DATA
		Used.Add(JsonValue);
#endif
		return false;
	}
}

bool FPsDataJsonDeserializer::ReadArray()
{
	TSharedPtr<FJsonValue> Value = ReadJsonValue();
	if (Value->Type == EJson::Array)
	{
		Values.Add(Value);
		return true;
	}
	return false;
}

bool FPsDataJsonDeserializer::ReadObject()
{
	TSharedPtr<FJsonValue> Value = ReadJsonValue();
	if (Value->Type == EJson::Object)
	{
		Values.Add(Value);
		return true;
	}
	return false;
}

bool FPsDataJsonDeserializer::ReadValue(int32& OutValue)
{
	return ReadJsonValue()->TryGetNumber(OutValue);
}

bool FPsDataJsonDeserializer::ReadValue(uint8& OutValue)
{
	int32 Out = 0;
	const bool bResult = ReadJsonValue()->TryGetNumber(Out);
	OutValue = static_cast<uint8>(Out);
	return bResult;
}

bool FPsDataJsonDeserializer::ReadValue(float& OutValue)
{
	double Out = 0;
	const bool bResult = ReadJsonValue()->TryGetNumber(Out);
	OutValue = static_cast<float>(Out);
	return bResult;
}

bool FPsDataJsonDeserializer::ReadValue(bool& OutValue)
{
	return ReadJsonValue()->TryGetBool(OutValue);
}

bool FPsDataJsonDeserializer::ReadValue(FString& OutValue)
{
	return ReadJsonValue()->TryGetString(OutValue);
}

bool FPsDataJsonDeserializer::ReadValue(FName& OutValue)
{
	FString Out;
	const bool bResult = ReadJsonValue()->TryGetString(Out);
	OutValue = FName(*Out);
	return bResult;
}

bool FPsDataJsonDeserializer::ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator)
{
	TSharedPtr<FJsonValue> Value = ReadJsonValue();
	if (Value->IsNull())
	{
		OutValue = nullptr;
		return true;
	}
	if (Value->Type == EJson::Object)
	{
		if (OutValue == nullptr)
		{
			OutValue = Allocator();
		}
		ReadObject();
		OutValue->DataDeserialize(this);
		PopObject();
		return true;
	}
	return false;
}

void FPsDataJsonDeserializer::PopKey(const FString& Key)
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Object);
	TSharedPtr<FJsonValue> JsonValue = Values.Last();
	auto& Iterator = KeysIterator.FindChecked(JsonValue);
	check(Iterator);
	check(Iterator.Key() == Key);
	++Iterator;
}

void FPsDataJsonDeserializer::PopIndex()
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Array);
	TSharedPtr<FJsonValue> JsonValue = Values.Last();
	auto& Iterator = IndicesIterator.FindChecked(JsonValue);
	check(Iterator);
	++Iterator;
}

void FPsDataJsonDeserializer::PopArray()
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Array);
	Values.Pop(false);
}

void FPsDataJsonDeserializer::PopObject()
{
	check(Values.Num() > 0 && Values.Last()->Type == EJson::Object);
	Values.Pop(false);
}
