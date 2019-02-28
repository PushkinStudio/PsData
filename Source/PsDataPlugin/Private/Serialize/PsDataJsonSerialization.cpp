// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataJsonSerialization.h"

#include "PsData.h"

/***********************************
 * FJsonDataSerializer
 ***********************************/

FPsDataJsonSerializer::FPsDataJsonSerializer(TSharedPtr<FJsonObject> InJson)
	: FPsDataSerializer()
	, RootJson(InJson)
{
	check(InJson.IsValid());

	JsonPath.Add(InJson);
	InJson->Values.Empty();
}

FPsDataJsonSerializer::FPsDataJsonSerializer()
	: FPsDataJsonSerializer(TSharedPtr<FJsonObject>(new FJsonObject()))
{
}

void FPsDataJsonSerializer::PushJson(TSharedPtr<FJsonObject> Json)
{
	JsonPath.Add(Json);
}

void FPsDataJsonSerializer::PopJson(TSharedPtr<FJsonObject> Json)
{
	check(JsonPath.Last() == Json);
	JsonPath.Pop(false);
}

TSharedPtr<FJsonObject> FPsDataJsonSerializer::GetLastJson()
{
	return JsonPath.Last();
}

TSharedPtr<FJsonObject>& FPsDataJsonSerializer::GetJson()
{
	return RootJson;
}

/***********************************
 * int32
 ***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, int32 Data)
{
	GetLastJson()->SetNumberField(Field->Name, Data);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for (const int32& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueNumber(Item)));
	}
	GetLastJson()->SetArrayField(Field->Name, Array);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for (auto& Pair : Data)
	{
		GetLastJson()->SetNumberField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Field->Name, MapPtr);
}

/***********************************
 * uint8
 ***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, uint8 Data)
{
	GetLastJson()->SetNumberField(Field->Name, Data);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for (const uint8& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueNumber(Item)));
	}
	GetLastJson()->SetArrayField(Field->Name, Array);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for (auto& Pair : Data)
	{
		GetLastJson()->SetNumberField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Field->Name, MapPtr);
}

/***********************************
 * float
 ***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, float Data)
{
	GetLastJson()->SetNumberField(Field->Name, Data);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<float>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for (const float& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueNumber(Item)));
	}
	GetLastJson()->SetArrayField(Field->Name, Array);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for (auto& Pair : Data)
	{
		GetLastJson()->SetNumberField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Field->Name, MapPtr);
}

/***********************************
 * string
 ***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const FString& Data)
{
	GetLastJson()->SetStringField(Field->Name, Data);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for (const FString& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueString(Item)));
	}
	GetLastJson()->SetArrayField(Field->Name, Array);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for (auto& Pair : Data)
	{
		GetLastJson()->SetStringField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Field->Name, MapPtr);
}

/***********************************
 * bool
 ***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, bool Data)
{
	GetLastJson()->SetBoolField(Field->Name, Data);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for (const bool& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueBoolean(Item)));
	}
	GetLastJson()->SetArrayField(Field->Name, Array);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for (auto& Pair : Data)
	{
		GetLastJson()->SetBoolField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Field->Name, MapPtr);
}

/***********************************
 * Data
 ***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, UPsData* Data)
{
	if (Data == nullptr)
	{
		GetLastJson()->SetField(Field->Name, MakeShareable(new FJsonValueNull()));
	}
	else
	{
		TSharedPtr<FJsonObject> Ptr = MakeShareable(new FJsonObject());
		GetLastJson()->SetObjectField(Field->Name, Ptr);

		PushJson(Ptr);
		Data->DataSerialize(this);
		PopJson(Ptr);
	}
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for (UPsData* Item : Data)
	{
		TSharedPtr<FJsonObject> Ptr = MakeShareable(new FJsonObject());
		Array.Add(MakeShareable(new FJsonValueObject(Ptr)));

		PushJson(Ptr);
		Item->DataSerialize(this);
		PopJson(Ptr);
	}
	GetLastJson()->SetArrayField(Field->Name, Array);
}

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for (auto& Pair : Data)
	{
		if (Pair.Value == nullptr)
		{
			GetLastJson()->SetField(Pair.Key, MakeShareable(new FJsonValueNull()));
		}
		else
		{
			TSharedPtr<FJsonObject> Ptr = MakeShareable(new FJsonObject());
			GetLastJson()->SetObjectField(Pair.Key, Ptr);

			PushJson(Ptr);
			Pair.Value->DataSerialize(this);
			PopJson(Ptr);
		}
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Field->Name, MapPtr);
}

/***********************************
* Custom
***********************************/

void FPsDataJsonSerializer::Serialize(const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data)
{
	GetLastJson()->Values.Add(Field->Name, Data);
}

/***********************************
 * FPsDataJsonDeserializer
 ***********************************/

FPsDataJsonDeserializer::FPsDataJsonDeserializer(TSharedPtr<FJsonObject> InJson)
	: FPsDataDeserializer()
	, RootJson(InJson)
{
	check(InJson.IsValid());
	JsonPath.Add(InJson);
}

void FPsDataJsonDeserializer::PushJson(TSharedPtr<FJsonObject> Json)
{
	JsonPath.Add(Json);
}

void FPsDataJsonDeserializer::PopJson(TSharedPtr<FJsonObject> Json)
{
	check(JsonPath.Last() == Json);
	JsonPath.Pop(false);
}

TSharedPtr<FJsonObject> FPsDataJsonDeserializer::GetLastJson()
{
	return JsonPath.Last();
}

FString FPsDataJsonDeserializer::GetStringType(EJson Type)
{
	switch (Type)
	{
	case EJson::None:
		return FString(TEXT("None"));
	case EJson::Null:
		return FString(TEXT("Null"));
	case EJson::String:
		return FString(TEXT("String"));
	case EJson::Number:
		return FString(TEXT("Number"));
	case EJson::Boolean:
		return FString(TEXT("Boolean"));
	case EJson::Array:
		return FString(TEXT("Array"));
	case EJson::Object:
		return FString(TEXT("Object"));
	}
	return FString(TEXT("Unknown"));
}

bool FPsDataJsonDeserializer::Has(const TSharedPtr<const FDataField>& Field)
{
	return GetLastJson()->HasField(Field->Name);
}

TSharedPtr<FJsonObject>& FPsDataJsonDeserializer::GetJson()
{
	return RootJson;
}

/***********************************
 * int32
 ***********************************/

int32 FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, int32 Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
		Data = 0;
	}
	else if (ValuePtr->Type == EJson::Number)
	{
		Data = ValuePtr->AsNumber();
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Int, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Data;
}

TArray<int32> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data, UClass* DataClass)
{
	TArray<int32> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());

		for (int32 Index = 0; Index < Array.Num(); ++Index)
		{
			TSharedPtr<FJsonValue> ItemValuePtr = Array[Index];
			if (ItemValuePtr->Type == EJson::Null)
			{
				Result.Add(0);
			}
			else if (ItemValuePtr->Type == EJson::Number)
			{
				Result.Add(ItemValuePtr->AsNumber());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Int, incoming json type: %s"), *Field->Name, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TMap<FString, int32> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data, UClass* DataClass)
{
	TMap<FString, int32> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for (auto& Pair : Object->Values)
		{
			if (Pair.Value->Type == EJson::Null)
			{
				Result.Add(Pair.Key, 0);
			}
			else if (Pair.Value->Type == EJson::Number)
			{
				Result.Add(Pair.Key, Pair.Value->AsNumber());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Int, incoming json type: %s"), *Field->Name, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

/***********************************
 * uint8
 ***********************************/

uint8 FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, uint8 Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
		Data = 0;
	}
	else if (ValuePtr->Type == EJson::Number)
	{
		Data = ValuePtr->AsNumber();
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Int, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Data;
}

TArray<uint8> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data, UClass* DataClass)
{
	TArray<uint8> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());

		for (int32 Index = 0; Index < Array.Num(); ++Index)
		{
			TSharedPtr<FJsonValue> ItemValuePtr = Array[Index];
			if (ItemValuePtr->Type == EJson::Null)
			{
				Result.Add(0);
			}
			else if (ItemValuePtr->Type == EJson::Number)
			{
				Result.Add(ItemValuePtr->AsNumber());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Int, incoming json type: %s"), *Field->Name, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TMap<FString, uint8> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data, UClass* DataClass)
{
	TMap<FString, uint8> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for (auto& Pair : Object->Values)
		{
			if (Pair.Value->Type == EJson::Null)
			{
				Result.Add(Pair.Key, 0);
			}
			else if (Pair.Value->Type == EJson::Number)
			{
				Result.Add(Pair.Key, Pair.Value->AsNumber());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Int, incoming json type: %s"), *Field->Name, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

/***********************************
 * float
 ***********************************/

float FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, float Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
		Data = 0.f;
	}
	else if (ValuePtr->Type == EJson::Number)
	{
		Data = ValuePtr->AsNumber();
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Float, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Data;
}

TArray<float> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<float>& Data, UClass* DataClass)
{
	TArray<float> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());

		for (int32 Index = 0; Index < Array.Num(); ++Index)
		{
			TSharedPtr<FJsonValue> ItemValuePtr = Array[Index];
			if (ItemValuePtr->Type == EJson::Null)
			{
				Result.Add(0.f);
			}
			else if (ItemValuePtr->Type == EJson::Number)
			{
				Result.Add(ItemValuePtr->AsNumber());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Float, incoming json type: %s"), *Field->Name, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TMap<FString, float> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data, UClass* DataClass)
{
	TMap<FString, float> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for (auto& Pair : Object->Values)
		{
			if (Pair.Value->Type == EJson::Null)
			{
				Result.Add(Pair.Key, 0.f);
			}
			else if (Pair.Value->Type == EJson::Number)
			{
				Result.Add(Pair.Key, Pair.Value->AsNumber());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Float, incoming json type: %s"), *Field->Name, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

/***********************************
 * string
 ***********************************/

FString FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Data, UClass* DataClass)
{
	FString Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::String)
	{
		Result = ValuePtr->AsString();
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as String, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TArray<FString> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data, UClass* DataClass)
{
	TArray<FString> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());

		for (int32 Index = 0; Index < Array.Num(); ++Index)
		{
			TSharedPtr<FJsonValue> ItemValuePtr = Array[Index];
			if (ItemValuePtr->Type == EJson::Null)
			{
				Result.Add(TEXT(""));
			}
			else if (ItemValuePtr->Type == EJson::String)
			{
				Result.Add(ItemValuePtr->AsString());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as String, incoming json type: %s"), *Field->Name, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TMap<FString, FString> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data, UClass* DataClass)
{
	TMap<FString, FString> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::String)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for (auto& Pair : Object->Values)
		{
			if (Pair.Value->Type == EJson::Null)
			{
				Result.Add(Pair.Key, TEXT(""));
			}
			else if (Pair.Value->Type == EJson::String)
			{
				Result.Add(Pair.Key, Pair.Value->AsString());
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as String, incoming json type: %s"), *Field->Name, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

/***********************************
 * bool
 ***********************************/

bool FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, bool Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
		Data = false;
	}
	else if (ValuePtr->Type == EJson::Boolean)
	{
		Data = ValuePtr->AsBool();
	}
	else if (ValuePtr->Type == EJson::Number)
	{
		Data = (ValuePtr->AsNumber() != 0.f);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Bool, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Data;
}

TArray<bool> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data, UClass* DataClass)
{
	TArray<bool> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());

		for (int32 Index = 0; Index < Array.Num(); ++Index)
		{
			TSharedPtr<FJsonValue> ItemValuePtr = Array[Index];
			if (ItemValuePtr->Type == EJson::Null)
			{
				Result.Add(false);
			}
			else if (ItemValuePtr->Type == EJson::Boolean)
			{
				Result.Add(ItemValuePtr->AsBool());
			}
			else if (ItemValuePtr->Type == EJson::Number)
			{
				Result.Add(ItemValuePtr->AsNumber() != 0.f);
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Bool, incoming json type: %s"), *Field->Name, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TMap<FString, bool> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data, UClass* DataClass)
{
	TMap<FString, bool> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for (auto& Pair : Object->Values)
		{
			if (Pair.Value->Type == EJson::Null)
			{
				Result.Add(Pair.Key, false);
			}
			else if (Pair.Value->Type == EJson::Boolean)
			{
				Result.Add(Pair.Key, Pair.Value->AsBool());
			}
			else if (Pair.Value->Type == EJson::Number)
			{
				Result.Add(Pair.Key, Pair.Value->AsNumber() != 0.f);
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Bool, incoming json type: %s"), *Field->Name, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

/***********************************
 * Data
 ***********************************/

UPsData* FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, UPsData* Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
		Data = nullptr;
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		if (Data == nullptr)
			Data = NewObject<UPsData>(Instance, DataClass);

		TSharedPtr<FJsonObject> Ptr = ValuePtr->AsObject();
		PushJson(Ptr);
		Data->DataDeserialize(this);
		PopJson(Ptr);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Object, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Data;
}

TArray<UPsData*> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data, UClass* DataClass)
{
	TArray<UPsData*> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());

		for (int32 Index = 0; Index < Array.Num(); ++Index)
		{
			TSharedPtr<FJsonValue> ItemValuePtr = Array[Index];
			if (ItemValuePtr->Type == EJson::Null)
			{
				Result.Add(nullptr);
			}
			else if (ItemValuePtr->Type == EJson::Object)
			{
				UPsData* Item = Data.IsValidIndex(Index) ? Data[Index] : NewObject<UPsData>(Instance, DataClass);

				TSharedPtr<FJsonObject> Ptr = ItemValuePtr->AsObject();
				PushJson(Ptr);
				Item->DataDeserialize(this);
				PopJson(Ptr);

				Result.Add(Item);
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Object, incoming json type: %s"), *Field->Name, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

TMap<FString, UPsData*> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data, UClass* DataClass)
{
	TMap<FString, UPsData*> Result;

	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Field->Name);
	if (ValuePtr->Type == EJson::Null)
	{
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for (auto& Pair : Object->Values)
		{
			if (Pair.Value->Type == EJson::Null)
			{
				Result.Add(Pair.Key, nullptr);
			}
			else if (Pair.Value->Type == EJson::Object)
			{
				UPsData* Item = Data.Contains(Pair.Key) ? *Data.Find(Pair.Key) : NewObject<UPsData>(Instance, DataClass);

				TSharedPtr<FJsonObject> Ptr = Pair.Value->AsObject();
				PushJson(Ptr);
				Item->DataDeserialize(this);
				PopJson(Ptr);

				Result.Add(Pair.Key, Item);
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Object, incoming json type: %s"), *Field->Name, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Field->Name, *GetStringType(ValuePtr->Type))
	}

	return Result;
}

/***********************************
 * Custom
 ***********************************/

TSharedPtr<FJsonValue> FPsDataJsonDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data, UClass* DataClass)
{
	return GetLastJson()->TryGetField(Field->Name);
}
