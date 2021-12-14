// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataField.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"

template <typename T, bool bConst>
struct FPsDataBaseMapProxy
{
private:
	friend struct FPsDataBaseMapProxy<T, true>;
	friend struct FPsDataBaseMapProxy<T, false>;

	PsDataTools::TConstValueType<PsDataTools::TDataProperty<TMap<FString, T>>*, bConst> Property;

	static PsDataTools::TDataProperty<TMap<FString, T>>* GetProperty(UPsData* Instance, const FDataField* Field)
	{
#if !UE_BUILD_SHIPPING
		TMap<FString, T>* Output = nullptr;
		PsDataTools::GetByField<true>(Instance, Field, Output);
#endif

		return static_cast<PsDataTools::TDataProperty<TMap<FString, T>>*>(PsDataTools::FPsDataFriend::GetProperty(Instance, Field->Index));
	}

	static const PsDataTools::TDataProperty<TMap<FString, T>>* GetProperty(const UPsData* Instance, const FDataField* Field)
	{
#if !UE_BUILD_SHIPPING
		TMap<FString, T>* Output = nullptr;
		PsDataTools::GetByField<true>(Instance, Field, Output);
#endif

		return static_cast<PsDataTools::TDataProperty<TMap<FString, T>>*>(PsDataTools::FPsDataFriend::GetProperty(Instance, Field->Index));
	}

protected:
	friend class UPsDataBlueprintMapProxy;

	FPsDataBaseMapProxy()
		: Property(nullptr)
	{
	}

public:
	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	FPsDataBaseMapProxy(UPsData* InInstance, const FDataField* InField)
		: Property(GetProperty(InInstance, InField))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	FPsDataBaseMapProxy(UPsData* InInstance, int32 Hash)
		: Property(GetProperty(InInstance, PsDataTools::FDataReflection::GetFieldsByClass(InInstance->GetClass())->GetFieldByHashChecked(Hash)))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	FPsDataBaseMapProxy(PsDataTools::TDataProperty<TMap<FString, T>>* InProperty)
		: Property(InProperty)
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<bOtherConst>::Type>
	FPsDataBaseMapProxy(const UPsData* InInstance, const FDataField* InField)
		: Property(GetProperty(InInstance, InField))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<bOtherConst>::Type>
	FPsDataBaseMapProxy(const UPsData* InInstance, int32 Hash)
		: Property(GetProperty(InInstance, PsDataTools::FDataReflection::GetFieldsByClass(InInstance->GetClass())->GetFieldByHashChecked(Hash)))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<bOtherConst>::Type>
	FPsDataBaseMapProxy(const PsDataTools::TDataProperty<TMap<FString, T>>* InProperty)
		: Property(InProperty)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseMapProxy(const FPsDataBaseMapProxy<T, bOtherConst>& MapProxy)
		: Property(MapProxy.Property)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseMapProxy(FPsDataBaseMapProxy<T, bOtherConst>&& MapProxy)
		: Property(MapProxy.Property)
	{
		MapProxy.Property = nullptr;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const FPsDataBaseMapProxy<T, bOtherConst>& MapProxy)
	{
		Property = MapProxy.Property;
	}

	const FDataField* GetField() const
	{
		return Property->GetField();
	}

	bool IsValid() const
	{
		return Property != nullptr;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Set(const TMap<FString, T>& NewMap)
	{
		Property->SetValue(NewMap);
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Add(const FString& Key, PsDataTools::TConstRefType<T, false> Element)
	{
		auto NewMap = Property->GetValue();
		NewMap.Add(Key, Element);
		Property->SetValue(NewMap);
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	bool Remove(const FString& Key)
	{
		auto& Map = Property->GetValue();
		if (Map.Find(Key))
		{
			auto NewMap = Map;
			NewMap.Remove(Key);
			Property->SetValue(NewMap);
			return true;
		}

		return false;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Empty()
	{
		Property->SetValue({});
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Reserve(int32 Number)
	{
		Property->GetValue().Reserve(Number);
	}

	auto Get() const
	{
		auto& Map = Property->GetValue();
		TMap<FString, PsDataTools::TConstValueType<T, bConst>> Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Key, Pair.Value);
		}
		return Result;
	}

	TArray<FString> GetKeys() const
	{
		auto& Map = Property->GetValue();
		TArray<FString> Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Key);
		}
		return Result;
	}

	auto GetValues() const
	{
		auto& Map = Property->GetValue();
		TArray<PsDataTools::TConstValueType<T, bConst>> Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Value);
		}
		return Result;
	}

	bool Contains(const FString& Key) const
	{
		return Property->GetValue().Contains(Key);
	}

	PsDataTools::TConstRefType<T*, bConst> Find(const FString& Key) const
	{
		return Property->GetValue().Find(Key);
	}

	PsDataTools::TConstRefType<T, bConst> FindChecked(const FString& Key) const
	{
		return Property->GetValue().FindChecked(Key);
	}

	int32 Num() const
	{
		return Property->GetValue().Num();
	}

	bool IsEmpty() const
	{
		return Num() == 0;
	}

	FPsDataBind Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return PsDataTools::FPsDataFriend::BindInternal(Property->GetOwner(), Type, Delegate, Flags, Property->GetField());
	}

	FPsDataBind Bind(const FString& Type, const FPsDataDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return PsDataTools::FPsDataFriend::BindInternal(Property->GetOwner(), Type, Delegate, Flags, Property->GetField());
	}

	void Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
	{
		PsDataTools::FPsDataFriend::UnbindInternal(Property->GetOwner(), Type, Delegate, Property->GetField());
	}

	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		PsDataTools::FPsDataFriend::UnbindInternal(Property->GetOwner(), Type, Delegate, Property->GetField());
	}

	PsDataTools::TConstRefType<T, bConst> operator[](const FString& Key) const
	{
		return Property->GetValue().FindChecked(Key);
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
			PsDataTools::TConstValueType<T, bIteratorConst> Value;

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
			auto& Map = Proxy.Property->GetValue();
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

		PsDataTools::TConstRefType<T, bIteratorConst> Value() const
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
