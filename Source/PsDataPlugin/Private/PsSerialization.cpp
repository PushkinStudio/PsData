// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsSerialization.h"
#include "PsData.h"

/***********************************
 * FDataSerializer
 ***********************************/

FDataSerializer::FDataSerializer(bool bInSerialize)
: bSerialize(bInSerialize)
{
	
}

bool FDataSerializer::Has(const FString& Key)
{
	return false;
}

bool FDataSerializer::IsSerialize()
{
	return bSerialize;
}

/***********************************
 * Data
 ***********************************/

void FDataSerializer::Serialize(const FString& Key, UPsData* Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TArray<UPsData*>& Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TMap<FString, UPsData*>& Data)
{
	
}

UPsData* FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, UPsData* Data, UClass* DataClass)
{
	return Data;
}

TArray<UPsData*> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<UPsData*>& Data, UClass* DataClass)
{
	return TArray<UPsData*>();
}

TMap<FString, UPsData*> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, UPsData*>& Data, UClass* DataClass)
{
	return TMap<FString, UPsData*>();
}

/***********************************
 * Int
 ***********************************/

void FDataSerializer::Serialize(const FString& Key, int32 Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TArray<int32>& Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TMap<FString, int32>& Data)
{
	
}

int32 FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, int32 Data, UClass* DataClass)
{
	return 0;
}

TArray<int32> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<int32>& Data, UClass* DataClass)
{
	return TArray<int32>();
}

TMap<FString, int32> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, int32>& Data, UClass* DataClass)
{
	return TMap<FString, int32>();
}

/***********************************
 * Float
 ***********************************/

void FDataSerializer::Serialize(const FString& Key, float Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TArray<float>& Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TMap<FString, float>& Data)
{
	
}

float FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, float Data, UClass* DataClass)
{
	return 0.f;
}

TArray<float> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<float>& Data, UClass* DataClass)
{
	return TArray<float>();
}

TMap<FString, float> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, float>& Data, UClass* DataClass)
{
	return TMap<FString, float>();
}

/***********************************
 * String
 ***********************************/

void FDataSerializer::Serialize(const FString& Key, const FString& Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TArray<FString>& Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TMap<FString, FString>& Data)
{
	
}

FString FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const FString& Data, UClass* DataClass)
{
	return FString();
}

TArray<FString> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<FString>& Data, UClass* DataClass)
{
	return TArray<FString>();
}

TMap<FString, FString> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, FString>& Data, UClass* DataClass)
{
	return TMap<FString, FString>();
}

/***********************************
 * Bool
 ***********************************/

void FDataSerializer::Serialize(const FString& Key, bool Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TArray<bool>& Data)
{
	
}

void FDataSerializer::Serialize(const FString& Key, const TMap<FString, bool>& Data)
{
	
}

bool FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, bool Data, UClass* DataClass)
{
	return false;
}

TArray<bool> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<bool>& Data, UClass* DataClass)
{
	return TArray<bool>();
}

TMap<FString, bool> FDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, bool>& Data, UClass* DataClass)
{
	return TMap<FString, bool>();
}

/***********************************
 * FJsonDataSerializer
 ***********************************/

FJsonDataSerializer::FJsonDataSerializer(TSharedPtr<FJsonObject> InJson, bool bInSerialize)
: FDataSerializer(bInSerialize)
, RootJson(InJson)
{
	check(InJson.IsValid())
	JsonPath.Add(InJson);
	
	if (bInSerialize)
	{
		InJson->Values.Empty();
	}
}

FJsonDataSerializer::FJsonDataSerializer(TSharedPtr<FJsonObject> InJson)
: FJsonDataSerializer(InJson, false)
{
	
}

FJsonDataSerializer::FJsonDataSerializer()
: FJsonDataSerializer(MakeShareable(new FJsonObject()), true)
{
	
}

void FJsonDataSerializer::PushJson(TSharedPtr<FJsonObject> Json)
{
	JsonPath.Add(Json);
}

void FJsonDataSerializer::PopJson(TSharedPtr<FJsonObject> Json)
{
	check(JsonPath.Last() == Json);
	JsonPath.Pop(false);
}

TSharedPtr<FJsonObject> FJsonDataSerializer::GetLastJson()
{
	return JsonPath.Last();
}

FString FJsonDataSerializer::GetStringType(EJson Type)
{
	switch(Type)
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

bool FJsonDataSerializer::Has(const FString& Key)
{
	if (bSerialize) return true;
	return GetLastJson()->HasField(Key);
}

TSharedPtr<FJsonObject>& FJsonDataSerializer::GetJson()
{
	return RootJson;
}

/***********************************
 * Data
 ***********************************/

void FJsonDataSerializer::Serialize(const FString& Key, UPsData* Data)
{
	if (Data == nullptr)
	{
		GetLastJson()->SetField(Key, MakeShareable(new FJsonValueNull()));
	}
	else
	{
		TSharedPtr<FJsonObject> Ptr = MakeShareable(new FJsonObject());
		GetLastJson()->SetObjectField(Key, Ptr);
		
		PushJson(Ptr);
		Data->DataSerialize(*this);
		PopJson(Ptr);
	}
}

void FJsonDataSerializer::Serialize(const FString& Key, const TArray<UPsData*>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for(UPsData* Item : Data)
	{
		TSharedPtr<FJsonObject> Ptr = MakeShareable(new FJsonObject());
		Array.Add(MakeShareable(new FJsonValueObject(Ptr)));
		
		PushJson(Ptr);
		Item->DataSerialize(*this);
		PopJson(Ptr);
	}
	GetLastJson()->SetArrayField(Key, Array);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TMap<FString, UPsData*>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for(auto& Pair : Data)
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
			Pair.Value->DataSerialize(*this);
			PopJson(Ptr);
		}
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Key, MapPtr);
}

UPsData* FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, UPsData* Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		Data = nullptr;
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		if (Data == nullptr) Data = NewObject<UPsData>(Instance, DataClass);
		
		TSharedPtr<FJsonObject> Ptr = ValuePtr->AsObject();
		PushJson(Ptr);
		Data->DataSerialize(*this);
		PopJson(Ptr);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Object, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Data;
}

TArray<UPsData*> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<UPsData*>& Data, UClass* DataClass)
{
	TArray<UPsData*> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());
		
		for(int32 Index = 0; Index < Array.Num(); ++Index)
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
				Item->DataSerialize(*this);
				PopJson(Ptr);
				
				Result.Add(Item);
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Object, incoming json type: %s"), *Key, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

TMap<FString, UPsData*> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, UPsData*>& Data, UClass* DataClass)
{
	TMap<FString, UPsData*> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());

		PushJson(Object);
		for(auto& Pair : Object->Values)
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
				Item->DataSerialize(*this);
				PopJson(Ptr);

				Result.Add(Pair.Key, Item);
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Object, incoming json type: %s"), *Key, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

/***********************************
 * Int
 ***********************************/

void FJsonDataSerializer::Serialize(const FString& Key, int32 Data)
{
	GetLastJson()->SetNumberField(Key, Data);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TArray<int32>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for(const int32& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueNumber(Item)));
	}
	GetLastJson()->SetArrayField(Key, Array);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TMap<FString, int32>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for(auto& Pair : Data)
	{
		GetLastJson()->SetNumberField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Key, MapPtr);
}

int32 FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, int32 Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
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
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Int, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Data;
}

TArray<int32> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<int32>& Data, UClass* DataClass)
{
	TArray<int32> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());
		
		for(int32 Index = 0; Index < Array.Num(); ++Index)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Int, incoming json type: %s"), *Key, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

TMap<FString, int32> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, int32>& Data, UClass* DataClass)
{
	TMap<FString, int32> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());
		
		PushJson(Object);
		for(auto& Pair : Object->Values)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Int, incoming json type: %s"), *Key, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

/***********************************
 * Float
 ***********************************/

void FJsonDataSerializer::Serialize(const FString& Key, float Data)
{
	GetLastJson()->SetNumberField(Key, Data);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TArray<float>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for(const float& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueNumber(Item)));
	}
	GetLastJson()->SetArrayField(Key, Array);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TMap<FString, float>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for(auto& Pair : Data)
	{
		GetLastJson()->SetNumberField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Key, MapPtr);
}

float FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, float Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
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
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Float, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Data;
}

TArray<float> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<float>& Data, UClass* DataClass)
{
	TArray<float> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());
		
		for(int32 Index = 0; Index < Array.Num(); ++Index)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Float, incoming json type: %s"), *Key, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

TMap<FString, float> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, float>& Data, UClass* DataClass)
{
	TMap<FString, float> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());
		
		PushJson(Object);
		for(auto& Pair : Object->Values)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Float, incoming json type: %s"), *Key, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

/***********************************
 * String
 ***********************************/

void FJsonDataSerializer::Serialize(const FString& Key, const FString& Data)
{
	GetLastJson()->SetStringField(Key, Data);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TArray<FString>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for(const FString& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueString(Item)));
	}
	GetLastJson()->SetArrayField(Key, Array);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TMap<FString, FString>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for(auto& Pair : Data)
	{
		GetLastJson()->SetStringField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Key, MapPtr);
}

FString FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const FString& Data, UClass* DataClass)
{
	FString Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::String)
	{
		Result = ValuePtr->AsString();
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as String, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

TArray<FString> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<FString>& Data, UClass* DataClass)
{
	TArray<FString> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());
		
		for(int32 Index = 0; Index < Array.Num(); ++Index)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as String, incoming json type: %s"), *Key, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

TMap<FString, FString> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, FString>& Data, UClass* DataClass)
{
	TMap<FString, FString> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::String)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());
		
		PushJson(Object);
		for(auto& Pair : Object->Values)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as String, incoming json type: %s"), *Key, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

/***********************************
 * Bool
 ***********************************/

void FJsonDataSerializer::Serialize(const FString& Key, bool Data)
{
	GetLastJson()->SetBoolField(Key, Data);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TArray<bool>& Data)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	Array.Reserve(Data.Num());
	for(const bool& Item : Data)
	{
		Array.Add(MakeShareable(new FJsonValueBoolean(Item)));
	}
	GetLastJson()->SetArrayField(Key, Array);
}

void FJsonDataSerializer::Serialize(const FString& Key, const TMap<FString, bool>& Data)
{
	TSharedPtr<FJsonObject> MapPtr = MakeShareable(new FJsonObject());
	PushJson(MapPtr);
	for(auto& Pair : Data)
	{
		GetLastJson()->SetBoolField(Pair.Key, Pair.Value);
	}
	PopJson(MapPtr);
	GetLastJson()->SetObjectField(Key, MapPtr);
}

bool FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, bool Data, UClass* DataClass)
{
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
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
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Bool, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Data;
}

TArray<bool> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TArray<bool>& Data, UClass* DataClass)
{
	TArray<bool> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> Array = ValuePtr->AsArray();
		Result.Reserve(Array.Num());
		
		for(int32 Index = 0; Index < Array.Num(); ++Index)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%d]\" as Bool, incoming json type: %s"), *Key, Index, *GetStringType(ValuePtr->Type))
			}
		}
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Array, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}

TMap<FString, bool> FJsonDataSerializer::Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, bool>& Data, UClass* DataClass)
{
	TMap<FString, bool> Result;
	
	TSharedPtr<FJsonValue> ValuePtr = GetLastJson()->TryGetField(Key);
	if (ValuePtr->Type == EJson::Null)
	{
		
	}
	else if (ValuePtr->Type == EJson::Object)
	{
		TSharedPtr<FJsonObject> Object = ValuePtr->AsObject();
		Result.Reserve(Object->Values.Num());
		
		PushJson(Object);
		for(auto& Pair : Object->Values)
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
				UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s[%s]\" as Bool, incoming json type: %s"), *Key, *Pair.Key, *GetStringType(ValuePtr->Type))
			}
		}
		PopJson(Object);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s\" as Map, incoming json type: %s"), *Key, *GetStringType(ValuePtr->Type))
	}
	
	return Result;
}
