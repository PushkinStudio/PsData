// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataTraits.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

/***********************************
 * 
 ***********************************/

namespace FDataReflectionTools
{
template <typename T>
bool UnsafeGet(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T*& OutValue);
template <typename T>
void UnsafeSet(UPsData* Instance, const TSharedPtr<const FDataField>& Field, typename TConstRef<T>::Type NewValue);
} // namespace FDataReflectionTools

/***********************************
* Comparison
***********************************/

template <typename T>
struct FTypeComparator
{
	static bool Compare(const T& Value0, const T& Value1)
	{
		return Value0 == Value1;
	}
};

template <>
struct FTypeComparator<FText>
{
	static bool Compare(const FText& Value0, const FText& Value1)
	{
		if (Value0.IsFromStringTable() == Value1.IsFromStringTable())
		{
			if (Value0.IsFromStringTable())
			{
				FName TableId0;
				FString Key0;
				FTextInspector::GetTableIdAndKey(Value0, TableId0, Key0);

				FName TableId1;
				FString Key1;
				FTextInspector::GetTableIdAndKey(Value1, TableId1, Key1);

				return TableId0 == TableId1 && Key0 == Key1;
			}
			else
			{
				return Value0.ToString() == Value1.ToString();
			}
		}
		return false;
	}
};

template <typename T>
struct FTypeComparator<TArray<T>>
{
	static bool Compare(const TArray<T>& Value0, const TArray<T>& Value1)
	{
		bool bChange = false;
		if (Value0.Num() == Value1.Num())
		{
			auto Iterator0 = Value0.CreateConstIterator();
			auto Iterator1 = Value1.CreateConstIterator();
			while (Iterator0)
			{
				if (!FTypeComparator<T>::Compare(*Iterator0, *Iterator1))
				{
					return false;
				}
				++Iterator0;
				++Iterator1;
			}
		}
		else
		{
			return false;
		}
		return true;
	}
};

template <typename T>
struct FTypeComparator<TMap<FString, T>>
{
	static bool Compare(const TMap<FString, T>& Value0, const TMap<FString, T>& Value1)
	{
		bool bChange = false;
		if (Value0.Num() == Value1.Num())
		{
			auto Iterator0 = Value0.CreateConstIterator();
			auto Iterator1 = Value1.CreateConstIterator();
			while (Iterator0)
			{
				if (!FTypeComparator<FString>::Compare(Iterator0.Key(), Iterator1.Key()) || !FTypeComparator<T>::Compare(Iterator0.Value(), Iterator1.Value()))
				{
					return false;
				}
				++Iterator0;
				++Iterator1;
			}
		}
		else
		{
			return false;
		}
		return true;
	}
};

/***********************************
* Memory for basic type
***********************************/

template <typename T>
struct FDataMemory : public FAbstractDataMemory
{
	T Value;
	FDataMemory()
		: Value(FDefaultValue<T>::GetDefault())
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		T NewValue = Deserializer->Deserialize(Instance, Field, Value, nullptr);
		FDataReflectionTools::UnsafeSet<T>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<T>(Instance, Field, FDefaultValue<T>::GetDefault());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T& Value, const T& NewValue)
	{
		if (FTypeComparator<T>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for basic type in array
***********************************/

template <typename T>
struct FDataMemory<TArray<T>> : public FAbstractDataMemory
{
	TArray<T> Value;
	FDataMemory() {}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TArray<T> NewValue = Deserializer->Deserialize(Instance, Field, Value, nullptr);
		FDataReflectionTools::UnsafeSet<TArray<T>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TArray<T>>(Instance, Field, TArray<T>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<T>& Value, const TArray<T>& NewValue)
	{
		if (FTypeComparator<TArray<T>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for basic type in map
***********************************/

template <typename T>
struct FDataMemory<TMap<FString, T>> : public FAbstractDataMemory
{
	TMap<FString, T> Value;
	FDataMemory()
		: Value()
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TMap<FString, T> NewValue = Deserializer->Deserialize(Instance, Field, Value, nullptr);
		FDataReflectionTools::UnsafeSet<TMap<FString, T>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TMap<FString, T>>(Instance, Field, TMap<FString, T>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, T>& Value, const TMap<FString, T>& NewValue)
	{
		if (FTypeComparator<TMap<FString, T>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for UPsData*
***********************************/

template <typename T>
struct FDataMemory<T*> : public FAbstractDataMemory
{
	static_assert(std::is_base_of<UPsData, T>::value, "Pointer must be only UPsData");

	T* Value;
	FDataMemory()
		: Value(nullptr)
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		T* NewValue = Cast<T>(Deserializer->Deserialize(Instance, Field, Value, T::StaticClass()));
		if (Value != NewValue)
		{
			FDataReflectionTools::UnsafeSet<T*>(Instance, Field, NewValue);
		}
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<T*>(Instance, Field, nullptr);
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T*& Value, T*& NewValue)
	{
		if (Value == NewValue)
			return false;
		if (Value)
		{
			FDataReflectionTools::FPsDataFriend::RemoveChild(Instance, Value);
		}

		Value = NewValue;

		if (NewValue)
		{
			FDataReflectionTools::FPsDataFriend::ChangeDataName(NewValue, Field->Name);
			FDataReflectionTools::FPsDataFriend::AddChild(Instance, NewValue);
		}
		return true;
	}
};

/***********************************
* Memory for TArray<UPsData*>
***********************************/

template <typename T>
struct FDataMemory<TArray<T*>> : public FAbstractDataMemory
{
	static_assert(std::is_base_of<UPsData, T>::value, "Pointer must be only UPsData");

	TArray<T*> Value;
	FDataMemory() {}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, *((TArray<UPsData*>*)((void*)&Value)));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TArray<UPsData*> NewValue = Deserializer->Deserialize(Instance, Field, *((TArray<UPsData*>*)((void*)&Value)), T::StaticClass());
		FDataReflectionTools::UnsafeSet<TArray<T*>>(Instance, Field, *((TArray<T*>*)((void*)&NewValue)));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TArray<T*>>(Instance, Field, TArray<T*>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<T*>& Value, const TArray<T*>& NewValue)
	{
		bool bChange = false;

		for (int32 i = 0; i < NewValue.Num(); ++i)
		{
			if (NewValue[i]->GetParent() != Instance)
			{
				FDataReflectionTools::FPsDataFriend::ChangeDataName(NewValue[i], FString::FromInt(i));
				FDataReflectionTools::FPsDataFriend::AddChild(Instance, NewValue[i]);
				bChange = true;
			}
		}

		for (int32 i = 0; i < Value.Num(); ++i)
		{
			if (!NewValue.Contains(Value[i])) //TODO: optimization
			{
				FDataReflectionTools::FPsDataFriend::RemoveChild(Instance, Value[i]);
				bChange = true;
			}
		}

		if (!bChange)
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for TMap<FString, UPsData*>
***********************************/

template <typename T>
struct FDataMemory<TMap<FString, T*>> : public FAbstractDataMemory
{
	static_assert(std::is_base_of<UPsData, T>::value, "Pointer must be only UPsData");

	TMap<FString, T*> Value;
	FDataMemory() {}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, *((TMap<FString, UPsData*>*)((void*)&Value)));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TMap<FString, UPsData*> NewValue = Deserializer->Deserialize(Instance, Field, *((TMap<FString, UPsData*>*)((void*)&Value)), T::StaticClass());
		FDataReflectionTools::UnsafeSet<TMap<FString, T*>>(Instance, Field, *((TMap<FString, T*>*)((void*)&NewValue)));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TMap<FString, T*>>(Instance, Field, TMap<FString, T*>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, T*>& Value, const TMap<FString, T*>& NewValue)
	{
		bool bChange = false;
		for (auto& Pair : NewValue)
		{
			if (Pair.Value->GetParent() != Instance)
			{
				FDataReflectionTools::FPsDataFriend::ChangeDataName(Pair.Value, Pair.Key);
				FDataReflectionTools::FPsDataFriend::AddChild(Instance, Pair.Value);
				bChange = true;
			}
		}

		for (auto& Pair : Value)
		{
			auto Find = NewValue.Find(Pair.Key);
			if (!Find)
			{
				FDataReflectionTools::FPsDataFriend::RemoveChild(Instance, Pair.Value);
				bChange = true;
			}
		}

		if (!bChange)
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for TSoftObjectPtr<T>
***********************************/

template <typename T>
struct FDataMemory<TSoftObjectPtr<T>> : public FAbstractDataMemory
{
	TSoftObjectPtr<T> Value;
	FDataMemory()
		: Value()
	{
	}
	virtual ~FDataMemory() {}

	static TSharedPtr<FJsonValue> SerializeHelper(const TSoftObjectPtr<T>& Value)
	{
		if (!Value.IsNull())
		{
			const FSoftObjectPath& SoftObjectPath = Value.GetUniqueID();
			TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
			JsonObject->SetStringField(TEXT("AssetPathName"), SoftObjectPath.GetAssetPathName().ToString());
			JsonObject->SetStringField(TEXT("SubPathString"), SoftObjectPath.GetSubPathString());
			return TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObject));
		}
		else
		{
			return TSharedPtr<FJsonValue>(new FJsonValueNull());
		}
	}

	static TSoftObjectPtr<T> DeserializeHelper(TSharedPtr<FJsonValue> JsonValue)
	{
		const TSharedPtr<FJsonObject>* JsonObjectPtr;
		if (JsonValue.IsValid() && !JsonValue->IsNull() && JsonValue->TryGetObject(JsonObjectPtr))
		{
			TSharedPtr<FJsonObject> JsonObject = *JsonObjectPtr;
			FSoftObjectPath SoftObjectPath(
				FName(*JsonObject->GetStringField(TEXT("AssetPathName"))),
				JsonObject->GetStringField(TEXT("SubPathString")));

			if (!SoftObjectPath.IsNull())
			{
				return TSoftObjectPtr<T>(SoftObjectPath);
			}
		}
		return TSoftObjectPtr<T>();
	}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, SerializeHelper(Value));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TSharedPtr<FJsonValue> JsonValue = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
		FDataReflectionTools::UnsafeSet<TSoftObjectPtr<T>>(Instance, Field, DeserializeHelper(JsonValue));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TSoftObjectPtr<T>>(Instance, Field, TSoftObjectPtr<T>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TSoftObjectPtr<T>& Value, const TSoftObjectPtr<T>& NewValue)
	{
		if (FTypeComparator<TSoftObjectPtr<T>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for TArray<TSoftObjectPtr<T>>
***********************************/

template <typename T>
struct FDataMemory<TArray<TSoftObjectPtr<T>>> : public FAbstractDataMemory
{
	TArray<TSoftObjectPtr<T>> Value;
	FDataMemory()
		: Value()
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		TArray<TSharedPtr<FJsonValue>> Array;
		for (const TSoftObjectPtr<T>& SoftObjectPtr : Value)
		{
			Array.Add(FDataMemory<TSoftObjectPtr<T>>::SerializeHelper(SoftObjectPtr));
		}
		Serializer->Serialize(Field, TSharedPtr<FJsonValue>(new FJsonValueArray(Array)));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TArray<TSoftObjectPtr<T>> NewValue;

		TSharedPtr<FJsonValue> JsonValueArray = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
		const TArray<TSharedPtr<FJsonValue>>* ArrayPtr;
		if (JsonValueArray.IsValid() && !JsonValueArray->IsNull() && JsonValueArray->TryGetArray(ArrayPtr))
		{
			TArray<TSharedPtr<FJsonValue>> Array = *ArrayPtr;
			for (TSharedPtr<FJsonValue>& JsonValue : Array)
			{
				NewValue.Add(FDataMemory<TSoftObjectPtr<T>>::DeserializeHelper(JsonValue));
			}
		}
		FDataReflectionTools::UnsafeSet<TArray<TSoftObjectPtr<T>>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TArray<TSoftObjectPtr<T>>>(Instance, Field, TArray<TSoftObjectPtr<T>>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<TSoftObjectPtr<T>>& Value, const TArray<TSoftObjectPtr<T>>& NewValue)
	{
		if (FTypeComparator<TArray<TSoftObjectPtr<T>>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for TMap<FString, TSoftObjectPtr<T>>
***********************************/

template <typename T>
struct FDataMemory<TMap<FString, TSoftObjectPtr<T>>> : public FAbstractDataMemory
{
	TMap<FString, TSoftObjectPtr<T>> Value;
	FDataMemory()
		: Value()
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		TSharedPtr<FJsonObject> Object(new FJsonObject());
		for (auto& Pair : Value)
		{
			Object->SetField(Pair.Key, FDataMemory<TSoftObjectPtr<T>>::SerializeHelper(Pair.Value));
		}
		Serializer->Serialize(Field, TSharedPtr<FJsonValue>(new FJsonValueObject(Object)));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TMap<FString, TSoftObjectPtr<T>> NewValue;

		TSharedPtr<FJsonValue> JsonValueObject = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
		const TSharedPtr<FJsonObject>* ObjectPtr;
		if (JsonValueObject.IsValid() && !JsonValueObject->IsNull() && JsonValueObject->TryGetObject(ObjectPtr))
		{
			TSharedPtr<FJsonObject> Object = *ObjectPtr;
			for (auto& Pair : Object->Values)
			{
				NewValue.Add(Pair.Key, FDataMemory<TSoftObjectPtr<T>>::DeserializeHelper(Pair.Value));
			}
		}
		FDataReflectionTools::UnsafeSet<TMap<FString, TSoftObjectPtr<T>>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TMap<FString, TSoftObjectPtr<T>>>(Instance, Field, TMap<FString, TSoftObjectPtr<T>>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, TSoftObjectPtr<T>>& Value, const TMap<FString, TSoftObjectPtr<T>>& NewValue)
	{
		if (FTypeComparator<TMap<FString, TSoftObjectPtr<T>>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
 * Memory for FText
 ***********************************/

template <>
struct FDataMemory<FText> : public FAbstractDataMemory
{
	FText Value;
	FDataMemory()
		: Value()
	{
	}

	virtual ~FDataMemory() {}

	static TSharedPtr<FJsonValue> SerializeHelper(const FText& Value)
	{
		if (Value.IsFromStringTable())
		{
			FName TableId;
			FString Key;
			FTextInspector::GetTableIdAndKey(Value, TableId, Key);

			TSharedPtr<FJsonObject> JsonObject(new FJsonObject());
			JsonObject->SetStringField(TEXT("TableId"), TableId.ToString());
			JsonObject->SetStringField(TEXT("Key"), Key);
			return TSharedPtr<FJsonValue>(new FJsonValueObject(JsonObject));
		}

		return TSharedPtr<FJsonValue>(new FJsonValueString(Value.ToString()));
	}

	static FText DeserializeHelper(TSharedPtr<FJsonValue> JsonValue)
	{
		if (JsonValue.IsValid() && !JsonValue->IsNull())
		{
			const TSharedPtr<FJsonObject>* JsonObjectPtr;
			if (JsonValue->TryGetObject(JsonObjectPtr))
			{
				TSharedPtr<FJsonObject> JsonObject = *JsonObjectPtr;
				return FText::FromStringTable(
					FName(*JsonObject->GetStringField(TEXT("TableId"))),
					JsonObject->GetStringField(TEXT("Key")));
			}

			FString String;
			if (JsonValue->TryGetString(String))
			{
				return FText::FromString(String);
			}
		}
		return FText();
	}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->Serialize(Field, SerializeHelper(Value));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TSharedPtr<FJsonValue> JsonValue = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
		FDataReflectionTools::UnsafeSet<FText>(Instance, Field, DeserializeHelper(JsonValue));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<FText>(Instance, Field, FText());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FText& Value, const FText& NewValue)
	{
		if (FTypeComparator<FText>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
 * Memory for TArray<FText>
 ***********************************/

template <>
struct FDataMemory<TArray<FText>> : public FAbstractDataMemory
{
	TArray<FText> Value;
	FDataMemory()
		: Value()
	{
	}

	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		TArray<TSharedPtr<FJsonValue>> Array;
		for (const FText& Text : Value)
		{
			Array.Add(FDataMemory<FText>::SerializeHelper(Text));
		}
		Serializer->Serialize(Field, TSharedPtr<FJsonValue>(new FJsonValueArray(Array)));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TArray<FText> NewValue;

		TSharedPtr<FJsonValue> JsonValueArray = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
		const TArray<TSharedPtr<FJsonValue>>* ArrayPtr;
		if (JsonValueArray.IsValid() && !JsonValueArray->IsNull() && JsonValueArray->TryGetArray(ArrayPtr))
		{
			TArray<TSharedPtr<FJsonValue>> Array = *ArrayPtr;
			for (TSharedPtr<FJsonValue>& JsonValue : Array)
			{
				NewValue.Add(FDataMemory<FText>::DeserializeHelper(JsonValue));
			}
		}
		FDataReflectionTools::UnsafeSet<TArray<FText>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TArray<FText>>(Instance, Field, TArray<FText>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<FText>& Value, const TArray<FText>& NewValue)
	{
		if (FTypeComparator<TArray<FText>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
 * Memory for TMap<FString, FText>
 ***********************************/

template <>
struct FDataMemory<TMap<FString, FText>> : public FAbstractDataMemory
{
	TMap<FString, FText> Value;
	FDataMemory()
		: Value()
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		TSharedPtr<FJsonObject> Object(new FJsonObject());
		for (auto& Pair : Value)
		{
			Object->SetField(Pair.Key, FDataMemory<FText>::SerializeHelper(Pair.Value));
		}
		Serializer->Serialize(Field, TSharedPtr<FJsonValue>(new FJsonValueObject(Object)));
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TMap<FString, FText> NewValue;

		TSharedPtr<FJsonValue> JsonValueObject = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
		const TSharedPtr<FJsonObject>* ObjectPtr;
		if (JsonValueObject.IsValid() && !JsonValueObject->IsNull() && JsonValueObject->TryGetObject(ObjectPtr))
		{
			TSharedPtr<FJsonObject> Object = *ObjectPtr;
			for (auto& Pair : Object->Values)
			{
				NewValue.Add(Pair.Key, FDataMemory<FText>::DeserializeHelper(Pair.Value));
			}
		}
		FDataReflectionTools::UnsafeSet<TMap<FString, FText>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<TMap<FString, FText>>(Instance, Field, TMap<FString, FText>());
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, FText>& Value, const TMap<FString, FText>& NewValue)
	{
		if (FTypeComparator<TMap<FString, FText>>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for Enum
***********************************/

template <typename T>
struct FEnumDataMemory : public FAbstractDataMemory
{
	static_assert(std::is_enum<T>::value, "Only \"enum class : uint8\" can be describe by DESCRIBE_ENUM macros");

	T Value;
	FEnumDataMemory()
		: Value(static_cast<T>(0))
	{
	}

	virtual ~FEnumDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
		if (Enum)
		{
			Serializer->Serialize(Field, TSharedPtr<FJsonValue>(new FJsonValueString(Enum->GetNameStringByValue(static_cast<int64>(Value)))));
		}
		else
		{
			Serializer->Serialize(Field, static_cast<uint8>(Value));
		}
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
		uint8 Result = 0;
		if (Enum)
		{
			TSharedPtr<FJsonValue> JsonValue = Deserializer->Deserialize(Instance, Field, TSharedPtr<FJsonValue>(), nullptr);
			if (JsonValue.IsValid() || JsonValue->IsNull())
			{
				if (JsonValue->Type == EJson::String)
				{
					Result = static_cast<uint8>(Enum->GetValueByNameString(JsonValue->AsString(), EGetByNameFlags::None));
				}
				else if (JsonValue->Type == EJson::Number)
				{
					Result = static_cast<uint8>(JsonValue->AsNumber());
				}
				else
				{
					Result = 0;
				}
			}
			else
			{
				Result = 0;
			}
		}
		else
		{
			Result = Deserializer->Deserialize(Instance, Field, static_cast<uint8>(Value), nullptr);
		}
		FDataReflectionTools::UnsafeSet<T>(Instance, Field, static_cast<T>(Result));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (!Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<T>(Instance, Field, static_cast<T>(0));
		}
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T& Value, const T& NewValue)
	{
		if (FTypeComparator<T>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

namespace FDataReflectionTools
{

/***********************************
 * UNSAFE GET PROPERTY
 ***********************************/

template <typename T>
bool UnsafeGet(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T*& OutValue)
{
	FDataMemory<T>* Memory = static_cast<FDataMemory<T>*>(FPsDataFriend::GetMemory(Instance)[Field->Index].Get());
	OutValue = &Memory->Value;
	return true;
}

/***********************************
 * UNSAFE SET PROPERTY
 ***********************************/

template <typename T>
void UnsafeSet(UPsData* Instance, const TSharedPtr<const FDataField>& Field, typename TConstRef<T>::Type NewValue)
{
	T* OldValue = nullptr;
	FDataMemory<T>* Memory = static_cast<FDataMemory<T>*>(FPsDataFriend::GetMemory(Instance)[Field->Index].Get());
	OldValue = &Memory->Value;

	if (FDataMemory<T>::Set(Instance, Field, *OldValue, NewValue))
	{
		UPsDataEvent::DispatchChange(Instance, Field);
	}
}
} // namespace FDataReflectionTools
