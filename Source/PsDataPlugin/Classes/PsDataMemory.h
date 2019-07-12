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

namespace FDataReflectionTools
{
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
} // namespace FDataReflectionTools

/***********************************
 * Serizlize/Deserialize
 ***********************************/

namespace FDataReflectionTools
{
template <typename T>
struct FTypeDefault
{
	static const T GetDefaultValue() { return T{}; }
};

template <>
struct FTypeDefault<int32>
{
	static const int32 GetDefaultValue() { return 0; }
};

template <>
struct FTypeDefault<uint8>
{
	static const uint8 GetDefaultValue() { return 0; }
};

template <>
struct FTypeDefault<float>
{
	static const float GetDefaultValue() { return 0.f; }
};

template <>
struct FTypeDefault<bool>
{
	static const bool GetDefaultValue() { return false; }
};
} // namespace FDataReflectionTools

/***********************************
 * Serizlize/Deserialize
 ***********************************/

namespace FDataReflectionTools
{
template <typename T>
struct FTypeSerializer
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const T& Value)
	{
		Serializer->WriteValue(Value);
	}
};

template <typename T>
struct FTypeDeserializer
{
	static T Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		T NewValue = FDataReflectionTools::FTypeDefault<T>::GetDefaultValue();
		Deserializer->ReadValue(NewValue);
		return NewValue;
	}

	static T Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const T& Value)
	{
		T NewValue = Value;
		Deserializer->ReadValue(NewValue);
		return NewValue;
	}
};

template <typename T>
struct FTypeSerializer<TArray<T>>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const TArray<T>& Value)
	{
		Serializer->WriteArray();
		for (const T& Element : Value)
		{
			FDataReflectionTools::FTypeSerializer<T>::Serialize(Field, Serializer, Element);
		}
		Serializer->PopArray();
	}
};

template <typename T>
struct FTypeDeserializer<TArray<T>>
{
	static TArray<T> Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const TArray<T>& Value)
	{
		TArray<T> NewValue;
		if (Deserializer->ReadArray())
		{
			int i = 0;
			while (Deserializer->ReadIndex())
			{
				if (Value.IsValidIndex(i))
				{
					NewValue.Add(FDataReflectionTools::FTypeDeserializer<T>::Deserialize(Field, Deserializer, Value[i]));
				}
				else
				{
					NewValue.Add(FDataReflectionTools::FTypeDeserializer<T>::Deserialize(Field, Deserializer));
				}
				i++;
				Deserializer->PopIndex();
			}
			Deserializer->PopArray();
		}
		return NewValue;
	}
};

template <typename T>
struct FTypeSerializer<TMap<FString, T>>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const TMap<FString, T>& Value, bool bUseSortedKeys = false)
	{
		Serializer->WriteObject();
		if (bUseSortedKeys)
		{
			//TODO: Bad problem solving:
			using FPair = typename TMap<FString, T>::ElementType;

			TArray<const FPair*> Pairs;
			Pairs.Reserve(Value.Num());
			for (auto& Pair : Value)
			{
				Pairs.Add(&Pair);
			}

			Pairs.Sort([](const FPair& A, const FPair& B) {
				return A.Key < B.Key;
			});

			for (auto PairPtr : Pairs)
			{
				Serializer->WriteKey(PairPtr->Key);
				FDataReflectionTools::FTypeSerializer<T>::Serialize(Field, Serializer, PairPtr->Value);
				Serializer->PopKey(PairPtr->Key);
			}
		}
		else
		{
			for (auto& Pair : Value)
			{
				Serializer->WriteKey(Pair.Key);
				FDataReflectionTools::FTypeSerializer<T>::Serialize(Field, Serializer, Pair.Value);
				Serializer->PopKey(Pair.Key);
			}
		}
		Serializer->PopObject();
	}
};

template <typename T>
struct FTypeDeserializer<TMap<FString, T>>
{
	static TMap<FString, T> Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const TMap<FString, T>& Value)
	{
		TMap<FString, T> NewValue;
		if (Deserializer->ReadObject())
		{
			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				if (Value.Contains(Key))
				{
					NewValue.Add(Key, FDataReflectionTools::FTypeDeserializer<T>::Deserialize(Field, Deserializer, Value[Key]));
				}
				else
				{
					NewValue.Add(Key, FDataReflectionTools::FTypeDeserializer<T>::Deserialize(Field, Deserializer));
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
		}
		return NewValue;
	}
};

template <>
struct FTypeSerializer<FText>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FText& Value)
	{
		static const FString TableIdParam(TEXT("TableId"));
		static const FString KeyParam(TEXT("Key"));

		if (Value.IsFromStringTable())
		{
			FName TableIdValue;
			FString KeyValue;
			FTextInspector::GetTableIdAndKey(Value, TableIdValue, KeyValue);

			Serializer->WriteObject();
			Serializer->WriteKey(TableIdParam);
			Serializer->WriteValue(TableIdValue);
			Serializer->PopKey(TableIdParam);
			Serializer->WriteKey(KeyParam);
			Serializer->WriteValue(KeyValue);
			Serializer->PopKey(KeyParam);
			Serializer->PopObject();
		}
		else
		{
			Serializer->WriteValue(Value.ToString());
		}
	}
};

template <>
struct FTypeDeserializer<FText>
{
	static FText Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		static const FString TableIdParam(TEXT("TableId"));
		static const FString KeyParam(TEXT("Key"));

		if (Deserializer->ReadObject())
		{
			FName TableIdValue;
			FString KeyValue;

			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				if (Key == TableIdParam)
				{
					Deserializer->ReadValue(TableIdValue);
				}
				else if (Key == KeyParam)
				{
					Deserializer->ReadValue(KeyValue);
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
			return FText::FromStringTable(TableIdValue, KeyValue);
		}
		else
		{
			FString String;
			Deserializer->ReadValue(String);
			return FText::FromString(String);
		}
	}

	static FText Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FText& Value)
	{
		return Deserialize(Field, Deserializer);
	}
};

template <>
struct FTypeSerializer<FName>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FName& Value)
	{
		Serializer->WriteValue(Value.ToString().ToLower());
	}
};

template <>
struct FTypeDeserializer<FName>
{
	static FName Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		FString String;
		Deserializer->ReadValue(String);
		return FName(*String.ToLower());
	}

	static FName Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FName& Value)
	{
		return Deserialize(Field, Deserializer);
	}
};

template <typename T>
struct FTypeSerializer<TSoftObjectPtr<T>>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const TSoftObjectPtr<T>& Value)
	{
		static const FString AssetPathNameParam(TEXT("AssetPathName"));
		static const FString SubPathStringParam(TEXT("SubPathString"));

		const FSoftObjectPath& SoftObjectPath = Value.GetUniqueID();
		Serializer->WriteObject();
		Serializer->WriteKey(AssetPathNameParam);
		Serializer->WriteValue(SoftObjectPath.GetAssetPathName());
		Serializer->PopKey(AssetPathNameParam);
		Serializer->WriteKey(SubPathStringParam);
		Serializer->WriteValue(SoftObjectPath.GetSubPathString());
		Serializer->PopKey(SubPathStringParam);
		Serializer->PopObject();
	}
};

template <typename T>
struct FTypeDeserializer<TSoftObjectPtr<T>>
{
	static TSoftObjectPtr<T> Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		static const FString AssetPathNameParam(TEXT("AssetPathName"));
		static const FString SubPathStringParam(TEXT("SubPathString"));

		if (Deserializer->ReadObject())
		{
			FName AssetPathNameValue;
			FString SubPathStringValue;

			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				if (Key == AssetPathNameParam)
				{
					Deserializer->ReadValue(AssetPathNameValue);
				}
				else if (Key == SubPathStringParam)
				{
					Deserializer->ReadValue(SubPathStringValue);
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
			return TSoftObjectPtr<T>(FSoftObjectPath(AssetPathNameValue, SubPathStringValue));
		}
		else
		{
			return TSoftObjectPtr<T>();
		}
	}

	static TSoftObjectPtr<T> Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const TSoftObjectPtr<T>& Value)
	{
		return Deserialize(Field, Deserializer);
	}
};

template <typename T>
struct FTypeSerializer<TSoftClassPtr<T>>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const TSoftClassPtr<T>& Value)
	{
		static const FString AssetPathNameParam(TEXT("AssetPathName"));
		static const FString SubPathStringParam(TEXT("SubPathString"));

		const FSoftObjectPath& SoftObjectPath = Value.GetUniqueID();
		Serializer->WriteObject();
		Serializer->WriteKey(AssetPathNameParam);
		Serializer->WriteValue(SoftObjectPath.GetAssetPathName());
		Serializer->PopKey(AssetPathNameParam);
		Serializer->WriteKey(SubPathStringParam);
		Serializer->WriteValue(SoftObjectPath.GetSubPathString());
		Serializer->PopKey(SubPathStringParam);
		Serializer->PopObject();
	}
};

template <typename T>
struct FTypeDeserializer<TSoftClassPtr<T>>
{
	static TSoftClassPtr<T> Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		static const FString AssetPathNameParam(TEXT("AssetPathName"));
		static const FString SubPathStringParam(TEXT("SubPathString"));

		FText NewValue;
		if (Deserializer->ReadObject())
		{
			FName AssetPathNameValue;
			FString SubPathStringValue;

			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				if (Key == AssetPathNameParam)
				{
					Deserializer->ReadValue(AssetPathNameValue);
				}
				else if (Key == SubPathStringParam)
				{
					Deserializer->ReadValue(SubPathStringValue);
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
			return TSoftClassPtr<T>(FSoftObjectPath(AssetPathNameValue, SubPathStringValue));
		}
		else
		{
			return TSoftClassPtr<T>();
		}
	}

	static TSoftClassPtr<T> Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const TSoftClassPtr<T>& Value)
	{
		return Deserialize(Field, Deserializer);
	}
};

template <>
struct FTypeSerializer<FLinearColor>
{
	static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FLinearColor& Value)
	{
		static const FString RParam(TEXT("r"));
		static const FString GParam(TEXT("g"));
		static const FString BParam(TEXT("b"));
		static const FString AParam(TEXT("a"));

		Serializer->WriteObject();
		Serializer->WriteKey(RParam);
		Serializer->WriteValue(Value.R);
		Serializer->PopKey(RParam);
		Serializer->WriteKey(GParam);
		Serializer->WriteValue(Value.G);
		Serializer->PopKey(GParam);
		Serializer->WriteKey(BParam);
		Serializer->WriteValue(Value.B);
		Serializer->PopKey(BParam);
		Serializer->WriteKey(AParam);
		Serializer->WriteValue(Value.A);
		Serializer->PopKey(AParam);
		Serializer->PopObject();
	}
};

template <>
struct FTypeDeserializer<FLinearColor>
{
	static FLinearColor Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		static const FString RParam(TEXT("r"));
		static const FString GParam(TEXT("g"));
		static const FString BParam(TEXT("b"));
		static const FString AParam(TEXT("a"));

		FLinearColor Result;
		if (Deserializer->ReadObject())
		{
			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				float Value = 0;
				if (Key == RParam && Deserializer->ReadValue(Value))
				{
					Result.R = Value;
				}
				else if (Key == GParam && Deserializer->ReadValue(Value))
				{
					Result.G = Value;
				}
				else if (Key == BParam && Deserializer->ReadValue(Value))
				{
					Result.B = Value;
				}
				else if (Key == AParam && Deserializer->ReadValue(Value))
				{
					Result.A = Value;
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
		}
		return Result;
	}

	static FLinearColor Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FLinearColor& Value)
	{
		return Deserialize(Field, Deserializer);
	}
};
} // namespace FDataReflectionTools

/***********************************
* Memory
***********************************/

template <typename T>
struct FDataMemory : public FAbstractDataMemory
{
	T Value;
	FDataMemory()
		: Value(FDataReflectionTools::FTypeDefault<T>::GetDefaultValue())
	{
	}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		FDataReflectionTools::FTypeSerializer<T>::Serialize(Field, Serializer, Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		FDataReflectionTools::UnsafeSet<T>(Instance, Field, FDataReflectionTools::FTypeDeserializer<T>::Deserialize(Field, Deserializer, Value));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		FDataReflectionTools::UnsafeSet<T>(Instance, Field, FDataReflectionTools::FTypeDefault<T>::GetDefaultValue());
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T& Value, const T& NewValue)
	{
		if (FDataReflectionTools::FTypeComparator<T>::Compare(Value, NewValue))
			return false;
		Value = NewValue;
		return true;
	}
};

/***********************************
* Memory for array
***********************************/

template <typename T>
struct FDataMemory<TArray<T>> : public FAbstractDataMemory
{
	TArray<T> Value;
	FDataMemory() {}
	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		FDataReflectionTools::FTypeSerializer<TArray<T>>::Serialize(Field, Serializer, Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		FDataReflectionTools::UnsafeSet<TArray<T>>(Instance, Field, FDataReflectionTools::FTypeDeserializer<TArray<T>>::Deserialize(Field, Deserializer, Value));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		FDataReflectionTools::UnsafeSet<TArray<T>>(Instance, Field, TArray<T>());
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<T>& Value, const TArray<T>& NewValue)
	{
		if (FDataReflectionTools::FTypeComparator<TArray<T>>::Compare(Value, NewValue))
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
		FDataReflectionTools::FTypeSerializer<TMap<FString, T>>::Serialize(Field, Serializer, Value, Serializer->UseSortedKeys());
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		FDataReflectionTools::UnsafeSet<TMap<FString, T>>(Instance, Field, FDataReflectionTools::FTypeDeserializer<TMap<FString, T>>::Deserialize(Field, Deserializer, Value));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		FDataReflectionTools::UnsafeSet<TMap<FString, T>>(Instance, Field, TMap<FString, T>());
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, T>& Value, const TMap<FString, T>& NewValue)
	{
		if (FDataReflectionTools::FTypeComparator<TMap<FString, T>>::Compare(Value, NewValue))
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
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	T* Value;

	FDataMemory()
		: Value(nullptr)
	{
	}

	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->WriteValue(Value);
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TWeakObjectPtr<UPsData> InstancePtr(Instance);
		FPsDataAllocator Allocator = [InstancePtr]() -> UPsData* { return NewObject<T>(InstancePtr.Get()); };

		UPsData* NewValue = Value;
		Deserializer->ReadValue(NewValue, Allocator);
		FDataReflectionTools::UnsafeSet<T*>(Instance, Field, Cast<T>(NewValue));
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		if (Field->Meta.bStrict)
		{
			FDataReflectionTools::UnsafeSet<T*>(Instance, Field, NewObject<T>(Instance, T::StaticClass()));
		}
		else
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
			FDataReflectionTools::FPsDataFriend::ChangeDataName(NewValue, Field->Name, TEXT(""));
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
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	TArray<T*> Value;

	FDataMemory() {}

	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->WriteArray();
		for (const T* Element : Value)
		{
			Serializer->WriteValue(Element);
		}
		Serializer->PopArray();
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TWeakObjectPtr<UPsData> InstancePtr(Instance);
		FPsDataAllocator Allocator = [InstancePtr]() -> UPsData* { return NewObject<T>(InstancePtr.Get()); };

		TArray<T*> NewValue;
		if (Deserializer->ReadArray())
		{
			int i = 0;
			while (Deserializer->ReadIndex())
			{
				UPsData* Element = nullptr;
				if (Value.IsValidIndex(i))
				{
					Element = Value[i];
				}
				if (Deserializer->ReadValue(Element, Allocator))
				{
					NewValue.Add(Cast<T>(Element));
				}
				i++;
				Deserializer->PopIndex();
			}
			Deserializer->PopArray();
		}
		FDataReflectionTools::UnsafeSet<TArray<T*>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		FDataReflectionTools::UnsafeSet<TArray<T*>>(Instance, Field, TArray<T*>());
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<T*>& Value, const TArray<T*>& NewValue)
	{
		bool bChange = false;

		for (int32 i = 0; i < NewValue.Num(); ++i)
		{
			if (NewValue[i]->GetParent() != Instance)
			{
				FDataReflectionTools::FPsDataFriend::ChangeDataName(NewValue[i], FString::FromInt(i), Field->Name);
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
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	TMap<FString, T*> Value;

	FDataMemory() {}

	virtual ~FDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const override
	{
		Serializer->WriteObject();
		for (auto& Pair : Value)
		{
			Serializer->WriteKey(Pair.Key);
			Serializer->WriteValue(Pair.Value);
			Serializer->PopKey(Pair.Key);
		}
		Serializer->PopObject();
	}

	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) override
	{
		TWeakObjectPtr<UPsData> InstancePtr(Instance);
		FPsDataAllocator Allocator = [InstancePtr]() -> UPsData* { return NewObject<T>(InstancePtr.Get()); };

		TMap<FString, T*> NewValue;
		if (Deserializer->ReadObject())
		{
			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				UPsData* Element = nullptr;
				if (Value.Contains(Key))
				{
					Element = Value.FindChecked(Key);
				}
				if (Deserializer->ReadValue(Element, Allocator))
				{
					NewValue.Add(Key, Cast<T>(Element));
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
		}

		FDataReflectionTools::UnsafeSet<TMap<FString, T*>>(Instance, Field, NewValue);
	}

	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) override
	{
		FDataReflectionTools::UnsafeSet<TMap<FString, T*>>(Instance, Field, TMap<FString, T*>());
	}

	static bool Set(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, T*>& Value, const TMap<FString, T*>& NewValue)
	{
		bool bChange = false;
		for (auto& Pair : NewValue)
		{
			if (Pair.Value->GetParent() != Instance)
			{
				FDataReflectionTools::FPsDataFriend::ChangeDataName(Pair.Value, Pair.Key, Field->Name);
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
