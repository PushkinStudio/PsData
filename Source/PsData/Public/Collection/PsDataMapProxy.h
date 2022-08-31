// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataField.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"

template <typename T, bool bConst>
struct TPsDataBaseMapProxy
{
private:
	friend struct TPsDataBaseMapProxy<T, true>;
	friend struct TPsDataBaseMapProxy<T, false>;

	PsDataTools::TConstValueType<PsDataTools::TDataProperty<TMap<FString, T>>*, bConst> Property;

public:
	using FKeyType = FString;

	TPsDataBaseMapProxy(PsDataTools::TConstValueType<PsDataTools::TDataProperty<TMap<FString, T>>*, bConst> InProperty)
		: Property(InProperty)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	TPsDataBaseMapProxy(const TPsDataBaseMapProxy<T, bOtherConst>& Proxy)
	{
		Property = Proxy.Property;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	TPsDataBaseMapProxy(TPsDataBaseMapProxy<T, bOtherConst>&& Proxy)
	{
		Property = Proxy.Property;
		Proxy.Property = nullptr;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const TPsDataBaseMapProxy<T, bOtherConst>& Proxy)
	{
		Property = Proxy.Property;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const TPsDataBaseMapProxy<T, bOtherConst>&& Proxy)
	{
		Property = Proxy.Property;
		Proxy.Property = nullptr;
	}

	const FDataField* GetField() const
	{
		return Property->GetField();
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

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	const TMap<FString, T>& GetConstRef()
	{
		return Property->GetValue();
	}

	TMap<FString, PsDataTools::TConstValueType<T, bConst>> GetCopy()
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

	TMap<FString, PsDataTools::TConstValueType<T, true>> GetCopy() const
	{
		auto& Map = Property->GetValue();
		TMap<FString, PsDataTools::TConstValueType<T, true>> Result;
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

	TArray<PsDataTools::TConstValueType<T, bConst>> GetValues()
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

	TArray<PsDataTools::TConstValueType<T, true>> GetValues() const
	{
		auto& Map = Property->GetValue();
		TArray<PsDataTools::TConstValueType<T, true>> Result;
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

	PsDataTools::TConstRefType<T*, bConst> Find(const FString& Key)
	{
		return Property->GetValue().Find(Key);
	}

	PsDataTools::TConstRefType<T*, true> Find(const FString& Key) const
	{
		return Property->GetValue().Find(Key);
	}

	PsDataTools::TConstRefType<T, bConst> FindChecked(const FString& Key)
	{
		return Property->GetValue().FindChecked(Key);
	}

	PsDataTools::TConstRefType<T, true> FindChecked(const FString& Key) const
	{
		return Property->GetValue().FindChecked(Key);
	}

	template <typename PredicateType>
	PsDataTools::TConstRefType<T*, bConst> FindByPredicate(const PredicateType& Predicate)
	{
		auto& Map = Property->GetValue();
		for (auto& Pair : Map)
		{
			PsDataTools::TConstRefType<T, true> Item = Pair.Value;
			if (Predicate(Item))
			{
				return &Pair.Value;
			}
		}

		return nullptr;
	}

	template <typename PredicateType>
	PsDataTools::TConstRefType<T*, true> FindByPredicate(const PredicateType& Predicate) const
	{
		auto& Map = Property->GetValue();
		for (auto& Pair : Map)
		{
			PsDataTools::TConstRefType<T, true> Item = Pair.Value;
			if (Predicate(Item))
			{
				return &Pair.Value;
			}
		}

		return nullptr;
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
		PsDataTools::FPsDataFriend::UnbindInternal(Property->GetOwner(), Type, Delegate);
	}

	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		PsDataTools::FPsDataFriend::UnbindInternal(Property->GetOwner(), Type, Delegate);
	}

	PsDataTools::TConstRefType<T, bConst> operator[](const FString& Key)
	{
		return Property->GetValue().FindChecked(Key);
	}

	PsDataTools::TConstRefType<T, true> operator[](const FString& Key) const
	{
		return Property->GetValue().FindChecked(Key);
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void operator=(const TMap<FString, T>& NewMap)
	{
		Set(NewMap);
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
		PsDataTools::TConstValueType<PsDataTools::TDataProperty<TMap<FString, T>>*, bIteratorConst> Property;
		TArray<TPair> Pairs;
		int32 Index;

	public:
		explicit TProxyIterator(PsDataTools::TConstValueType<PsDataTools::TDataProperty<TMap<FString, T>>*, bIteratorConst> InProperty, bool bEnd = false)
			: Property(InProperty)
			, Index(0)
		{
			auto& Map = Property->GetValue();
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

		template <bool bOtherConst = bIteratorConst,
			typename = typename TEnableIf<!bOtherConst>::Type>
		bool RemoveCurrent()
		{
			auto& Map = Property->GetValue();
			const auto& CurrentKey = Key();
			if (Map.Find(CurrentKey))
			{
				auto NewMap = Map;
				NewMap.Remove(CurrentKey);
				Property->SetValue(NewMap);
				return true;
			}

			return false;
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

	TProxyIterator<false> CreateIterator()
	{
		return TProxyIterator<false>(Property);
	}

	TProxyIterator<true> CreateConstIterator() const
	{
		return TProxyIterator<true>(Property);
	}

	TProxyIterator<bConst> begin() { return TProxyIterator<bConst>(Property); }
	TProxyIterator<bConst> end() { return TProxyIterator<bConst>(Property, true); }
	TProxyIterator<true> begin() const { return TProxyIterator<true>(Property); }
	TProxyIterator<true> end() const { return TProxyIterator<true>(Property, true); }
};

template <typename T>
using TPsDataMapProxy = TPsDataBaseMapProxy<T, false>;
template <typename T>
using TPsDataConstMapProxy = TPsDataBaseMapProxy<T, true>;

template <typename T, bool bConst>
struct TPsDataBaseMapProxy<TArray<T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct TPsDataBaseMapProxy<TMap<FString, T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};

namespace Algo
{
template <typename T, bool bConst, typename PredicateType>
FORCEINLINE PsDataTools::TConstRefType<T*, bConst> FindByPredicate(TPsDataBaseMapProxy<T, bConst> Proxy, const PredicateType& Predicate)
{
	return Proxy.FindByPredicate(Predicate);
}

template <typename T, typename PredicateType>
FORCEINLINE PsDataTools::TConstRefType<T*, true> FindByPredicate(const TPsDataBaseMapProxy<T, true> Proxy, const PredicateType& Predicate)
{
	return Proxy.FindByPredicate(Predicate);
}
} // namespace Algo