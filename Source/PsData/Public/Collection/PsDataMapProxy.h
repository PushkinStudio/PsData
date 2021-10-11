// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataHardObjectPtr.h"
#include "PsDataTraits.h"
#include "Types/PsData_UPsData.h"

#include "CoreMinimal.h"

template <typename T, bool bConst>
struct FPsDataBaseMapProxy
{
private:
	friend struct FPsDataBaseMapProxy<T, true>;
	friend struct FPsDataBaseMapProxy<T, false>;

	THardObjectPtr<UPsData> Instance;
	PsDataTools::FDataProperty<TMap<FString, T>>* Property;

	static PsDataTools::FDataProperty<TMap<FString, T>>* GetProperty(UPsData* Instance, const TSharedPtr<const FDataField>& Field)
	{
		TMap<FString, T>* Output = nullptr;
		PsDataTools::GetByField(Instance, Field, Output);
		check(Output);

		return static_cast<PsDataTools::FDataProperty<TMap<FString, T>>*>(PsDataTools::FPsDataFriend::GetProperties(Instance)[Field->Index]);
	}

protected:
	friend class UPsDataBlueprintMapProxy;

	FPsDataBaseMapProxy()
		: Property(nullptr)
	{
	}

public:
	FPsDataBaseMapProxy(UPsData* InInstance, const TSharedPtr<const FDataField>& InField)
		: Instance(InInstance)
		, Property(GetProperty(InInstance, InField))
	{
		check(IsValid());
	}

	FPsDataBaseMapProxy(UPsData* InInstance, int32 Hash)
		: Instance(InInstance)
		, Property(GetProperty(InInstance, PsDataTools::FDataReflection::GetFieldByHash(InInstance->GetClass(), Hash)))
	{
		check(IsValid());
	}

	FPsDataBaseMapProxy(UPsData* InInstance, const PsDataTools::FDataProperty<TMap<FString, T>>* InProperty)
		: Instance(InInstance)
		, Property(const_cast<PsDataTools::FDataProperty<TMap<FString, T>>*>(InProperty)) // TODO: const_cast
	{
		check(IsValid());
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseMapProxy(const FPsDataBaseMapProxy<T, bOtherConst>& MapProxy)
		: Instance(MapProxy.Instance)
		, Property(MapProxy.Property)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseMapProxy(FPsDataBaseMapProxy<T, bOtherConst>&& MapProxy)
		: Instance(std::move(MapProxy.Instance))
		, Property(MapProxy.Property)
	{
		MapProxy.Instance = nullptr;
		MapProxy.Property = nullptr;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const FPsDataBaseMapProxy<T, bOtherConst>& MapProxy)
	{
		Instance = MapProxy.Instance;
		Property = MapProxy.Property;
	}

	TSharedPtr<const FDataField> GetField() const
	{
		return Property->GetField();
	}

	bool IsValid() const
	{
		return Instance.IsValid() && Property;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Set(const TMap<FString, T>& NewMap)
	{
		Property->Set(NewMap, Instance.Get());
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Add(const FString& Key, typename PsDataTools::TConstRef<T, false>::Type Element)
	{
		auto NewMap = Property->Get();
		NewMap.Add(Key, Element);
		Property->Set(NewMap, Instance.Get());
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	bool Remove(const FString& Key)
	{
		auto& Map = Property->Get();
		if (Map.Find(Key))
		{
			auto NewMap = Map;
			NewMap.Remove(Key);
			Property->Set(NewMap, Instance.Get());
			return true;
		}

		return false;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Empty()
	{
		Property->Set({}, Instance.Get());
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Reserve(int32 Number)
	{
		Property->Get().Reserve(Number);
	}

	typename PsDataTools::TConstValue<TMap<FString, T>, bConst>::Type Get() const
	{
		auto& Map = Property->Get();
		typename PsDataTools::TConstValue<TMap<FString, T>, bConst>::Type Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Key, Pair.Value);
		}
		return Result;
	}

	TArray<FString> GetKeys() const
	{
		auto& Map = Property->Get();
		TArray<FString> Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Key);
		}
		return Result;
	}

	typename PsDataTools::TConstValue<TArray<T>, bConst>::Type GetValues() const
	{
		auto& Map = Property->Get();
		typename PsDataTools::TConstValue<TArray<T>, bConst>::Type Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Value);
		}
		return Result;
	}

	bool Contains(const FString& Key) const
	{
		return Property->Get().Contains(Key);
	}

	typename PsDataTools::TConstRef<T*, bConst>::Type Find(const FString& Key) const
	{
		return Property->Get().Find(Key);
	}

	typename PsDataTools::TConstRef<T, bConst>::Type FindChecked(const FString& Key) const
	{
		return Property->Get().FindChecked(Key);
	}

	int32 Num() const
	{
		return Property->Get().Num();
	}

	bool IsEmpty() const
	{
		return Num() == 0;
	}

	FPsDataBind Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
	{
		return Instance->BindInternal(Type, Delegate, Property->GetField());
	}

	FPsDataBind Bind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		return Instance->BindInternal(Type, Delegate, Property->GetField());
	}

	void Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
	{
		Instance->UnbindInternal(Type, Delegate, Property->GetField());
	}

	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		Instance->UnbindInternal(Type, Delegate, Property->GetField());
	}

	typename PsDataTools::TConstRef<T, bConst>::Type operator[](const FString& Key) const
	{
		return Property->Get().FindChecked(Key);
	}

	/***********************************
     * TProxyIterator
     ***********************************/

public:
	template <bool bIteratorConst>
	struct TProxyIterator
	{
		struct TPair
		{
			const FString Key;
			typename PsDataTools::TConstValue<T, bIteratorConst>::Type Value;

			TPair(const TPair&) = delete;
			TPair& operator=(const TPair&) = delete;
			TPair(TPair&&) = default;
		};

	private:
		friend struct FPsDataBaseMapProxy;

		FPsDataBaseMapProxy<T, bIteratorConst> Proxy;
		TArray<TPair> Pairs;
		int32 Index;

		TProxyIterator(const FPsDataBaseMapProxy<T, bIteratorConst>& InProxy, bool bEnd = false)
			: Proxy(InProxy)
			, Index(0)
		{
			auto& Map = Proxy.Property->Get();
			if (bEnd)
			{
				Index = Map.Num();
			}
			else
			{
				Pairs.Reserve(Map.Num());
				for (const auto& Pair : Map)
				{
					Pairs.Add({Pair.Key, Pair.Value});
				}
			}
		}

	public:
		template <bool bOtherConst = bIteratorConst,
			typename = typename TEnableIf<!bOtherConst>::Type>
		bool RemoveCurrent()
		{
			return Proxy.Remove(Key());
		}

		TProxyIterator& operator++()
		{
			++Index;
			return *this;
		}

		explicit operator bool() const
		{
			return Index < Pairs.Num();
		}

		bool operator!() const
		{
			return Index >= Pairs.Num();
		}

		friend bool operator==(const TProxyIterator& Lhs, const TProxyIterator& Rhs)
		{
			return Lhs.Index == Rhs.Index;
		}

		friend bool operator!=(const TProxyIterator& Lhs, const TProxyIterator& Rhs)
		{
			return Lhs.Index != Rhs.Index;
		}

		const FString& Key() const
		{
			return Pairs[Index].Key;
		}

		typename PsDataTools::TConstRef<T, bIteratorConst>::Type Value() const
		{
			return Pairs[Index].Value;
		}

		const TPair& operator*() const
		{
			return Pairs[Index];
		}

		const TPair& operator->() const
		{
			return Pairs[Index];
		}
	};

	TProxyIterator<bConst> CreateIterator() const
	{
		return TProxyIterator<bConst>(*this);
	}

	TProxyIterator<true> CreateConstIterator() const
	{
		return TProxyIterator<true>(*this);
	}

	TProxyIterator<bConst> begin() { return TProxyIterator<bConst>(*this); }
	TProxyIterator<bConst> end() { return TProxyIterator<bConst>(*this, true); }
	TProxyIterator<true> begin() const { return TProxyIterator<true>(*this); }
	TProxyIterator<true> end() const { return TProxyIterator<true>(*this, true); }
};

template <class T>
using FPsDataMapProxy = FPsDataBaseMapProxy<T, false>;
template <class T>
using FPsDataConstMapProxy = FPsDataBaseMapProxy<T, true>;

template <typename T, bool bConst>
struct FPsDataBaseMapProxy<TArray<T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct FPsDataBaseMapProxy<TMap<FString, T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};
