// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataTraits.h"
#include "PsDataUtils.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "UObject/GCObject.h"

namespace PsDataTools
{

template <typename T>
UPsData* CastToPsData(T* Value)
{
#if !UE_BUILD_SHIPPING
	const auto Class = FindUClass<T>();
	check(Class && Class->IsChildOf(UPsData::StaticClass()));
#endif // UE_BUILD_SHIPPING

	return static_cast<UPsData*>(static_cast<void*>(Value));
}

/***********************************
 * Comparison
 ***********************************/

template <typename T>
struct TTypeComparator
{
	static bool Compare(const T& Value0, const T& Value1)
	{
		return Value0 == Value1;
	}
};

template <typename T>
struct TTypeComparator<T*>
{
	static bool Compare(const T* Value0, const T* Value1)
	{
		return Value0 == Value1;
	}
};

template <typename T>
struct TTypeComparator<TArray<T>>
{
	static bool Compare(const TArray<T>& Value0, const TArray<T>& Value1)
	{
		if (Value0.Num() == Value1.Num())
		{
			auto Iterator0 = Value0.CreateConstIterator();
			auto Iterator1 = Value1.CreateConstIterator();
			while (Iterator0)
			{
				if (!TTypeComparator<T>::Compare(*Iterator0, *Iterator1))
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
struct TTypeComparator<TMap<FString, T>>
{
	static bool Compare(const TMap<FString, T>& Value0, const TMap<FString, T>& Value1)
	{
		if (Value0.Num() == Value1.Num())
		{
			auto Iterator0 = Value0.CreateConstIterator();
			auto Iterator1 = Value1.CreateConstIterator();
			while (Iterator0)
			{
				if (!TTypeComparator<FString>::Compare(Iterator0.Key(), Iterator1.Key()) || !TTypeComparator<T>::Compare(Iterator0.Value(), Iterator1.Value()))
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
 * Default value
 ***********************************/

template <typename T>
struct TTypeDefault
{
	static T GetDefaultValue() { return T(); }
};

template <typename T>
struct TTypeDefault<T*>
{
	static T* GetDefaultValue() { return nullptr; }
};

template <typename T>
struct TTypeDefault<TArray<T>>
{
	static TArray<T> GetDefaultValue() { return {}; }
};

template <typename T>
struct TTypeDefault<TMap<FString, T>>
{
	static TMap<FString, T> GetDefaultValue() { return {}; }
};

/***********************************
 * Default value
 ***********************************/

template <typename T>
struct TTypeToString
{
};

template <typename T>
struct TTypeToString<TArray<T>>
{
	static TArray<FString> ToString(const TArray<T>& Value)
	{
		TArray<FString> Result;
		for (const T& Item : Value)
		{
			Result.Add(TTypeToString<T>::ToString(Item));
		}
		return Result;
	}
};

template <typename T>
struct TTypeToString<TMap<FString, T>>
{
	static TMap<FString, FString> ToString(const TMap<FString, T>& Value)
	{
		TMap<FString, FString> Result;
		for (const auto& Pair : Value)
		{
			Result.Add(Pair.Key, TTypeToString<T>::ToString(Pair.Value));
		}
		return Result;
	}
};

/***********************************
 * Serialize/Deserialize
 ***********************************/

template <typename T>
struct TTypeSerializer
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const T& Value) = delete;
};

template <typename T>
struct TTypeDeserializer
{
	static T Deserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const T& Value) = delete;
};

template <typename T, typename L>
struct TTypeSerializerExtended
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const T& Value)
	{
		L::TypeSerialize(Instance, Field, Serializer, Value);
	}
};

template <typename T, typename L>
struct TTypeDeserializerExtended
{
	static T Deserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const T& Value)
	{
		return L::TypeDeserialize(Instance, Field, Deserializer, Value);
	}
};

template <typename T>
struct TTypeSerializer<TArray<T>>
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const TArray<T>& Value)
	{
		Serializer->WriteArray();
		for (const T& Element : Value)
		{
			TTypeSerializer<T>::Serialize(Instance, Field, Serializer, Element);
		}
		Serializer->PopArray();
	}
};

template <typename T>
struct TTypeDeserializer<TArray<T>>
{
	static TArray<T> Deserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const TArray<T>& Value)
	{
		TArray<T> NewValue;
		if (Deserializer->ReadArray())
		{
			int i = 0;
			while (Deserializer->ReadIndex())
			{
				if (Value.IsValidIndex(i))
				{
					NewValue.Add(TTypeDeserializer<T>::Deserialize(Instance, Field, Deserializer, Value[i]));
				}
				else
				{
					NewValue.Add(TTypeDeserializer<T>::Deserialize(Instance, Field, Deserializer, TTypeDefault<T>::GetDefaultValue()));
				}
				i++;
				Deserializer->PopIndex();
			}
			Deserializer->PopArray();
		}
		else
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *FType<TArray<T>>::Type())
		}

		return NewValue;
	}
};

template <typename T>
struct TTypeSerializer<TMap<FString, T>>
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const TMap<FString, T>& Value)
	{
		Serializer->WriteObject();
		for (auto& Pair : Value)
		{
			Serializer->WriteKey(Pair.Key);
			TTypeSerializer<T>::Serialize(Instance, Field, Serializer, Pair.Value);
			Serializer->PopKey(Pair.Key);
		}
		Serializer->PopObject();
	}
};

template <typename T>
struct TTypeDeserializer<TMap<FString, T>>
{
	static TMap<FString, T> Deserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const TMap<FString, T>& Value)
	{
		TMap<FString, T> NewValue;
		if (Deserializer->ReadObject())
		{
			FString Key;
			while (Deserializer->ReadKey(Key))
			{
				if (Value.Contains(Key))
				{
					NewValue.Add(Key, TTypeDeserializer<T>::Deserialize(Instance, Field, Deserializer, Value[Key]));
				}
				else
				{
					NewValue.Add(Key, TTypeDeserializer<T>::Deserialize(Instance, Field, Deserializer, TTypeDefault<T>::GetDefaultValue()));
				}
				Deserializer->PopKey(Key);
			}
			Deserializer->PopObject();
		}
		else
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *FType<TMap<FString, T>>::Type())
		}
		return NewValue;
	}
};

/***********************************
 * Event Storage
 ***********************************/

template <typename T>
struct TDataPropertyEventStorage : public FAbstractDataPropertyEventStorage
{
	using Type = T;

private:
	Type PreviousValue;
	Type Value;

	static TSharedPtr<FAbstractDataPropertyEventStorage> MakeEventStorage(const FDataField* Field, const T& OldValue, const T& NewValue)
	{
		if (!Field->Meta.bEventStorage)
		{
			return nullptr;
		}

		const auto Storage = new TDataPropertyEventStorage<T>();
		Storage->Field = Field;
		Storage->PreviousValue = OldValue;
		Storage->Value = NewValue;
		return TSharedPtr<FAbstractDataPropertyEventStorage>(Storage);
	}

	template <typename K>
	friend struct TDataProperty;

public:
	const Type& GetPreviousValue() const
	{
		return PreviousValue;
	}

	const Type& GetValue() const
	{
		return Value;
	}
};

template <typename T>
struct TDataPropertyEventStorage<T*> : public FAbstractDataPropertyEventStorage, public FGCObject
{
	using Type = typename TConstValue<T*, true>::Type;

private:
	using ObjectPtrType = TObjectPtr<UObject>;

	ObjectPtrType PreviousValue;
	ObjectPtrType Value;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(PreviousValue);
		Collector.AddReferencedObject(Value);
	}

	static TSharedPtr<FAbstractDataPropertyEventStorage> MakeEventStorage(const FDataField* Field, T* OldValue, T* NewValue)
	{
		if (!Field->Meta.bEventStorage)
		{
			return nullptr;
		}

		const auto Storage = new TDataPropertyEventStorage<T*>();
		Storage->Field = Field;
		Storage->PreviousValue = reinterpret_cast<UObject*>(OldValue);
		Storage->Value = reinterpret_cast<UObject*>(NewValue);
		return TSharedPtr<FAbstractDataPropertyEventStorage>(Storage);
	}

	virtual FString GetReferencerName() const override
	{
		return TEXT("TDataPropertyEventStorage");
	}

	template <typename K>
	friend struct TDataProperty;

public:
	Type GetPreviousValue() const
	{
		return reinterpret_cast<Type>(ObjectPtrDecay(PreviousValue));
	}

	Type GetValue() const
	{
		return reinterpret_cast<Type>(ObjectPtrDecay(Value));
	}
};

template <typename T>
struct TDataPropertyEventStorage<TArray<T*>> : public FAbstractDataPropertyEventStorage, public FGCObject
{
	using Type = typename TConstValue<T*, true>::Type;
	using ArrayType = TArray<Type>;

private:
	using ObjectPtrArrayType = TArray<TObjectPtr<UObject>>;

	ObjectPtrArrayType PreviousValue;
	ObjectPtrArrayType Value;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObjects(PreviousValue);
		Collector.AddReferencedObjects(Value);
	}

	static ObjectPtrArrayType Wrap(const TArray<T*>& Items)
	{
		ObjectPtrArrayType Result;
		for (auto Item : Items)
		{
			Result.Add(reinterpret_cast<UObject*>(Item));
		}

		return Result;
	}

	static TSharedPtr<FAbstractDataPropertyEventStorage> MakeEventStorage(const FDataField* Field, const TArray<T*>& OldValue, const TArray<T*>& NewValue)
	{
		if (!Field->Meta.bEventStorage)
		{
			return nullptr;
		}

		const auto Storage = new TDataPropertyEventStorage<TArray<T*>>();
		Storage->Field = Field;
		Storage->PreviousValue = Wrap(OldValue);
		Storage->Value = Wrap(NewValue);
		return TSharedPtr<FAbstractDataPropertyEventStorage>(Storage);
	}

	virtual FString GetReferencerName() const override
	{
		return TEXT("TDataPropertyEventStorage");
	}

	template <typename K>
	friend struct TDataProperty;

public:
	const ArrayType& GetPreviousValue() const
	{
		return *reinterpret_cast<ArrayType*>(&ObjectPtrDecay(PreviousValue));
	}

	const ArrayType& GetValue() const
	{
		return *reinterpret_cast<ArrayType*>(&ObjectPtrDecay(Value));
	}
};

template <typename T>
struct TDataPropertyEventStorage<TMap<FString, T*>> : public FAbstractDataPropertyEventStorage, public FGCObject
{
	using Type = typename TConstValue<T*, true>::Type;
	using MapType = TMap<FString, Type>;

private:
	using ObjectPtrMapType = TMap<FString, TObjectPtr<UObject>>;

	ObjectPtrMapType PreviousValue;
	ObjectPtrMapType Value;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObjects(PreviousValue);
		Collector.AddReferencedObjects(Value);
	}

	static ObjectPtrMapType Wrap(const TMap<FString, T*>& Items)
	{
		ObjectPtrMapType Result;
		for (auto Pair : Items)
		{
			Result.Add(Pair.Key, reinterpret_cast<UObject*>(Pair.Value));
		}
		return Result;
	}

	static TSharedPtr<FAbstractDataPropertyEventStorage> MakeEventStorage(const FDataField* Field, const TMap<FString, T*>& OldValue, const TMap<FString, T*>& NewValue)
	{
		if (!Field->Meta.bEventStorage)
		{
			return nullptr;
		}

		const auto Storage = new TDataPropertyEventStorage<TMap<FString, T*>>();
		Storage->Field = Field;
		Storage->PreviousValue = Wrap(OldValue);
		Storage->Value = Wrap(NewValue);
		return TSharedPtr<FAbstractDataPropertyEventStorage>(Storage);
	}

	virtual FString GetReferencerName() const override
	{
		return TEXT("TDataPropertyEventStorage");
	}

	template <typename K>
	friend struct TDataProperty;

public:
	const MapType& GetPreviousValue() const
	{
		return *reinterpret_cast<MapType*>(&ObjectPtrDecay(PreviousValue));
	}

	const MapType& GetValue() const
	{
		return *reinterpret_cast<MapType*>(&ObjectPtrDecay(Value));
	}
};

/***********************************
 * Property
 ***********************************/

template <typename T>
struct TDataProperty : public FAbstractDataProperty
{
	T Value;

	TDataProperty()
		: Value(TTypeDefault<T>::GetDefaultValue())
	{
	}

	virtual ~TDataProperty() override {}

	virtual void Serialize(FPsDataSerializer* Serializer) const override
	{
		TTypeSerializer<T>::Serialize(GetOwner(), GetField(), Serializer, GetValue());
	}

	virtual void Deserialize(FPsDataDeserializer* Deserializer) override
	{
		SetValue(TTypeDeserializer<T>::Deserialize(GetOwner(), GetField(), Deserializer, Value));
	}

	virtual void Reset() override
	{
		SetValue(TTypeDefault<T>::GetDefaultValue());
	}

	virtual bool IsDefault() const override
	{
		return TTypeComparator<T>::Compare(Value, TTypeDefault<T>::GetDefaultValue());
	}

	const T& GetValue() const
	{
		return Value;
	}

	T& GetValue()
	{
		return Value;
	}

	void SetValue(const T& NewValue)
	{
		FPsDataEventScopeGuard EventGuard;

		if (TTypeComparator<T>::Compare(Value, NewValue))
		{
			return;
		}

		const auto Storage = TDataPropertyEventStorage<T>::MakeEventStorage(GetField(), Value, NewValue);

		Value = NewValue;

		FPsDataFriend::Changed(GetOwner(), GetField(), Storage);
	}

	auto GetEventStorageValue(const UPsDataEvent* Event) const
	{
		const auto Storage = Event->GetStorage();
		check(Storage && Storage->GetField() == GetField());

		return static_cast<TDataPropertyEventStorage<T> const*>(Storage.Get());
	}
};

/***********************************
 * Property for array
 ***********************************/

template <typename T>
struct TDataProperty<TArray<T>> : public FAbstractDataProperty
{
	TArray<T> Value;

	TDataProperty()
	{
	}

	virtual ~TDataProperty() override {}

	virtual void Serialize(FPsDataSerializer* Serializer) const override
	{
		TTypeSerializer<TArray<T>>::Serialize(GetOwner(), GetField(), Serializer, GetValue());
	}

	virtual void Deserialize(FPsDataDeserializer* Deserializer) override
	{
		SetValue(TTypeDeserializer<TArray<T>>::Deserialize(GetOwner(), GetField(), Deserializer, Value));
	}

	virtual void Reset() override
	{
		SetValue({});
	}

	virtual bool IsDefault() const override
	{
		return Value.Num() == 0;
	}

	const TArray<T>& GetValue() const
	{
		return Value;
	}

	TArray<T>& GetValue()
	{
		return Value;
	}

	void SetValue(const TArray<T>& NewValue)
	{
		FPsDataEventScopeGuard EventGuard;

		if (TTypeComparator<TArray<T>>::Compare(Value, NewValue))
		{
			return;
		}

		const auto Storage = TDataPropertyEventStorage<TArray<T>>::MakeEventStorage(GetField(), Value, NewValue);

		Value = NewValue;

		FPsDataFriend::Changed(GetOwner(), GetField(), Storage);
	}

	auto GetEventStorageValue(const UPsDataEvent* Event) const
	{
		const auto Storage = Event->GetStorage();
		check(Storage && Storage->GetField() == GetField());

		return static_cast<TDataPropertyEventStorage<TArray<T>> const*>(Storage.Get());
	}
};

/***********************************
 * Property for basic type in map
 ***********************************/

template <typename T>
struct TDataProperty<TMap<FString, T>> : public FAbstractDataProperty
{
	mutable TMap<FString, T> Value;
	mutable bool bSorted;

	TDataProperty()
		: bSorted(true)
	{
	}

	virtual ~TDataProperty() override {}

	virtual void Serialize(FPsDataSerializer* Serializer) const override
	{
		TTypeSerializer<TMap<FString, T>>::Serialize(GetOwner(), GetField(), Serializer, GetValue());
	}

	virtual void Deserialize(FPsDataDeserializer* Deserializer) override
	{
		SetValue(TTypeDeserializer<TMap<FString, T>>::Deserialize(GetOwner(), GetField(), Deserializer, Value));
	}

	virtual void Reset() override
	{
		SetValue({});
	}

	virtual bool IsDefault() const override
	{
		return Value.Num() == 0;
	}

	const TMap<FString, T>& GetValue() const
	{
		Sort();
		return Value;
	}

	TMap<FString, T>& GetValue()
	{
		Sort();
		return Value;
	}

	void SetValue(const TMap<FString, T>& NewValue)
	{
		FPsDataEventScopeGuard EventGuard;

		if (TTypeComparator<TMap<FString, T>>::Compare(Value, NewValue))
		{
			return;
		}

#if !UE_BUILD_SHIPPING
		for (const auto& Pair : NewValue)
		{
			if (!IsValidKey(Pair.Key))
			{
				UE_LOG(LogData, Fatal, TEXT("Illegal key \"%s\" for map %s::%s"), *Pair.Key, *GetOwner()->GetClass()->GetName(), *GetField()->Name);
			}
		}
#endif

		const auto Storage = TDataPropertyEventStorage<TMap<FString, T>>::MakeEventStorage(GetField(), Value, NewValue);

		Value = NewValue;
		bSorted = false;

		FPsDataFriend::Changed(GetOwner(), GetField(), Storage);
	}

	void Sort() const
	{
		if (!bSorted)
		{
			bSorted = true;
			Value.KeyStableSort([](const FString& A, const FString& B) {
				return A < B;
			});
		}
	}

	auto GetEventStorageValue(const UPsDataEvent* Event) const
	{
		const auto Storage = Event->GetStorage();
		check(Storage && Storage->GetField() == GetField());

		return static_cast<TDataPropertyEventStorage<TMap<FString, T>> const*>(Storage.Get());
	}
};

/***********************************
 * Property for UPsData*
 ***********************************/

template <typename T>
struct TDataProperty<T*> : public FAbstractDataProperty
{
	T* Value;

	TDataProperty()
		: Value(nullptr)
	{
	}

	virtual ~TDataProperty() override {}

	virtual void Serialize(FPsDataSerializer* Serializer) const override
	{
		TTypeSerializer<T*>::Serialize(GetOwner(), GetField(), Serializer, GetValue());
	}

	virtual void Deserialize(FPsDataDeserializer* Deserializer) override
	{
		SetValue(TTypeDeserializer<T*>::Deserialize(GetOwner(), GetField(), Deserializer, Value));
	}

	virtual void Reset() override
	{
		if (GetField()->Meta.bStrict)
		{
			CastToPsData(Value)->Reset();
		}
		else
		{
			SetValue(nullptr);
		}
	}

	virtual void Allocate() override
	{
		const FPsDataAllocator Allocator(GetField()->Context->GetUEType(), GetOwner());
		SetValue(static_cast<T*>(static_cast<void*>(Allocator())));
	}

	virtual bool IsDefault() const override
	{
		return Value == nullptr;
	}

	const T* GetValue() const
	{
		return Value;
	}

	T*& GetValue()
	{
		return Value;
	}

	void SetValue(T* NewValue)
	{
		FPsDataEventScopeGuard EventGuard;

		const auto Field = GetField();
		if (Field->Meta.bStrict)
		{
			check(NewValue != nullptr);
		}

		if (Value == NewValue)
		{
			return;
		}

		if (Value)
		{
			FPsDataFriend::RemoveChild(GetOwner(), CastToPsData(Value));
		}

		const auto Storage = TDataPropertyEventStorage<T*>::MakeEventStorage(GetField(), Value, NewValue);

		Value = NewValue;

		if (NewValue)
		{
			FPsDataFriend::ChangeDataName(CastToPsData(NewValue), Field->Name, TEXT(""));
			FPsDataFriend::AddChild(GetOwner(), CastToPsData(NewValue));
		}

		FPsDataFriend::Changed(GetOwner(), Field, Storage);
	}

	auto GetEventStorageValue(const UPsDataEvent* Event) const
	{
		const auto Storage = Event->GetStorage();
		check(Storage && Storage->GetField() == GetField());

		return static_cast<TDataPropertyEventStorage<T*> const*>(Storage.Get());
	}
};

/***********************************
 * Property for TArray<UPsData*>
 ***********************************/

template <typename T>
struct TDataProperty<TArray<T*>> : public FAbstractDataProperty
{
	TArray<T*> Value;

	TDataProperty()
	{
	}

	virtual ~TDataProperty() override {}

	virtual void Serialize(FPsDataSerializer* Serializer) const override
	{
		TTypeSerializer<TArray<T*>>::Serialize(GetOwner(), GetField(), Serializer, GetValue());
	}

	virtual void Deserialize(FPsDataDeserializer* Deserializer) override
	{
		SetValue(TTypeDeserializer<TArray<T*>>::Deserialize(GetOwner(), GetField(), Deserializer, Value));
	}

	virtual void Reset() override
	{
		SetValue({});
	}

	virtual bool IsDefault() const override
	{
		return Value.Num() == 0;
	}

	const TArray<T*>& GetValue() const
	{
		return Value;
	}

	TArray<T*>& GetValue()
	{
		return Value;
	}

	void SetValue(const TArray<T*>& NewValue)
	{
		FPsDataEventScopeGuard EventGuard;

		bool bChange = false;
		const auto Field = GetField();

		for (int32 i = 0; i < NewValue.Num(); ++i)
		{
			auto NewData = CastToPsData(NewValue[i]);
			FPsDataFriend::ChangeDataName(NewData, FString::FromInt(i), Field->Name);

			if (NewData->GetParent() != GetOwner())
			{
				FPsDataFriend::AddChild(GetOwner(), NewData);
				bChange = true;
			}
		}

		for (int32 i = 0; i < Value.Num(); ++i)
		{
			if (!NewValue.Contains(Value[i])) // TODO: optimization
			{
				FPsDataFriend::RemoveChild(GetOwner(), CastToPsData(Value[i]));
				bChange = true;
			}
		}

		if (!bChange)
		{
			return;
		}

		const auto Storage = TDataPropertyEventStorage<TArray<T*>>::MakeEventStorage(GetField(), Value, NewValue);

		Value = NewValue;

		FPsDataFriend::Changed(GetOwner(), Field, Storage);
	}

	auto GetEventStorageValue(const UPsDataEvent* Event) const
	{
		const auto Storage = Event->GetStorage();
		check(Storage && Storage->GetField() == GetField());

		return static_cast<TDataPropertyEventStorage<TArray<T*>> const*>(Storage.Get());
	}
};

/***********************************
 * Property for TMap<FString, UPsData*>
 ***********************************/

template <typename T>
struct TDataProperty<TMap<FString, T*>> : public FAbstractDataProperty
{
	mutable TMap<FString, T*> Value;
	mutable bool bSorted;

	TDataProperty()
		: bSorted(true)
	{
	}

	virtual ~TDataProperty() override {}

	virtual void Serialize(FPsDataSerializer* Serializer) const override
	{
		TTypeSerializer<TMap<FString, T*>>::Serialize(GetOwner(), GetField(), Serializer, GetValue());
	}

	virtual void Deserialize(FPsDataDeserializer* Deserializer) override
	{
		SetValue(TTypeDeserializer<TMap<FString, T*>>::Deserialize(GetOwner(), GetField(), Deserializer, Value));
	}

	virtual void Reset() override
	{
		SetValue({});
	}

	virtual bool IsDefault() const override
	{
		return Value.Num() == 0;
	}

	const TMap<FString, T*>& GetValue() const
	{
		Sort();
		return Value;
	}

	TMap<FString, T*>& GetValue()
	{
		Sort();
		return Value;
	}

	void SetValue(const TMap<FString, T*>& NewValue)
	{
		FPsDataEventScopeGuard EventGuard;

#if !UE_BUILD_SHIPPING
		for (const auto& Pair : NewValue)
		{
			if (!IsValidKey(Pair.Key))
			{
				UE_LOG(LogData, Fatal, TEXT("Illegal key \"%s\" for map %s::%s"), *Pair.Key, *GetOwner()->GetClass()->GetName(), *GetField()->Name);
			}
		}
#endif

		bool bChange = false;
		const auto Field = GetField();

		for (auto& Pair : NewValue)
		{
			auto NewData = CastToPsData(Pair.Value);
			if (NewData->GetParent() != GetOwner())
			{
				FPsDataFriend::ChangeDataName(NewData, Pair.Key, Field->Name);
				FPsDataFriend::AddChild(GetOwner(), NewData);
				bChange = true;
			}
		}

		for (auto& Pair : Value)
		{
			auto Find = NewValue.Find(Pair.Key);
			if (!Find)
			{
				FPsDataFriend::RemoveChild(GetOwner(), CastToPsData(Pair.Value));
				bChange = true;
			}
		}

		if (!bChange)
		{
			return;
		}

		const auto Storage = TDataPropertyEventStorage<TMap<FString, T*>>::MakeEventStorage(GetField(), Value, NewValue);

		Value = NewValue;
		bSorted = false;

		FPsDataFriend::Changed(GetOwner(), Field, Storage);
	}

	void Sort() const
	{
		if (!bSorted)
		{
			bSorted = true;
			Value.KeyStableSort([](const FString& A, const FString& B) {
				return A < B;
			});
		}
	}

	auto GetEventStorageValue(const UPsDataEvent* Event) const
	{
		const auto Storage = Event->GetStorage();
		check(Storage && Storage->GetField() == GetField());

		return static_cast<TDataPropertyEventStorage<TMap<FString, T*>> const*>(Storage.Get());
	}
};

/***********************************
 * Unsafe get property
 ***********************************/

template <typename T>
void UnsafeGetByIndex(UPsData* Instance, int32 Index, T*& OutValue)
{
	TDataProperty<T>* Property = static_cast<TDataProperty<T>*>(FPsDataFriend::GetProperties(Instance)[Index]);
	OutValue = &Property->GetValue();
}

template <typename T>
void UnsafeGet(UPsData* Instance, const FDataField* Field, T*& OutValue)
{
	UnsafeGetByIndex<T>(Instance, Field->Index, OutValue);
}

/***********************************
 * Unsafe set property
 ***********************************/

template <typename T>
void UnsafeSetByIndex(UPsData* Instance, int32 Index, TConstRefType<T, false> NewValue)
{
	TDataProperty<T>* Property = static_cast<TDataProperty<T>*>(FPsDataFriend::GetProperties(Instance)[Index]);
	Property->SetValue(NewValue);
}

template <typename T>
void UnsafeSet(UPsData* Instance, const FDataField* Field, TConstRefType<T, false> NewValue)
{
	UnsafeSetByIndex(Instance, Field->Index, NewValue);
}

} // namespace PsDataTools
