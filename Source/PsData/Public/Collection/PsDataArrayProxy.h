// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataField.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"

template <typename T, bool bConst>
struct TPsDataBaseArrayProxy
{
private:
	friend struct TPsDataBaseArrayProxy<T, true>;
	friend struct TPsDataBaseArrayProxy<T, false>;

	PsDataTools::TConstValueType<PsDataTools::TDataProperty<TArray<T>>*, bConst> Property;

public:
	using FKeyType = int32;

	TPsDataBaseArrayProxy(PsDataTools::TConstValueType<PsDataTools::TDataProperty<TArray<T>>*, bConst> InProperty)
		: Property(InProperty)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	TPsDataBaseArrayProxy(const TPsDataBaseArrayProxy<T, bOtherConst>& Proxy)
	{
		Property = Proxy.Property;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	TPsDataBaseArrayProxy(TPsDataBaseArrayProxy<T, bOtherConst>&& Proxy)
	{
		Property = Proxy.Property;
		Proxy.Property = nullptr;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const TPsDataBaseArrayProxy<T, bOtherConst>& Proxy)
	{
		Property = Proxy.Property;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const TPsDataBaseArrayProxy<T, bOtherConst>&& Proxy)
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
	void Set(const TArray<T>& NewArray)
	{
		Property->SetValue(NewArray);
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	int32 Add(PsDataTools::TConstRefType<T, false> Element)
	{
		auto NewArray = Property->GetValue();
		auto Index = NewArray.Add(Element);
		Property->SetValue(NewArray);

		return Index;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Insert(PsDataTools::TConstRefType<T, false> Element, int32 Index)
	{
		auto NewArray = Property->GetValue();
		NewArray.Insert(Element, Index);
		Property->SetValue(NewArray);
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void RemoveAt(int32 Index, bool bAllowShrinking = false)
	{
		auto NewArray = Property->GetValue();
		NewArray.RemoveAt(Index, 1, bAllowShrinking);
		Property->SetValue(NewArray);
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	int32 Remove(PsDataTools::TConstRefType<T, false> Element, bool bAllowShrinking = false)
	{
		auto Index = Property->GetValue().Find(Element);
		if (Index == INDEX_NONE)
		{
			return INDEX_NONE;
		}

		auto NewArray = Property->GetValue();
		NewArray.RemoveAt(Index, 1, bAllowShrinking);
		Property->SetValue(NewArray);

		return Index;
	}

	template <typename PredicateType, bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	int32 RemoveAll(const PredicateType& Predicate)
	{
		auto NewArray = Property->GetValue();

		int32 RemovedElements = 0;
		for (auto It = NewArray.CreateIterator(); It; ++It)
		{
			PsDataTools::TConstRefType<T> Item = *It;
			if (Predicate(Item))
			{
				It.RemoveCurrent();
				++RemovedElements;
			}
		}

		Property->SetValue(NewArray);
		return RemovedElements;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	PsDataTools::TConstRefType<T, bConst> Set(PsDataTools::TConstRefType<T, false> Element, int32 Index)
	{
		auto NewArray = Property->GetValue();
		auto& OldElement = NewArray[Index];
		NewArray[Index] = Element;
		Property->SetValue(NewArray);
		return OldElement;
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
	const TArray<T>& GetConstRef()
	{
		return Property->GetValue();
	}

	TArray<PsDataTools::TConstValueType<T, bConst>> GetCopy()
	{
		auto& Array = Property->GetValue();
		TArray<PsDataTools::TConstValueType<T, bConst>> Result;
		Result.Reserve(Array.Num());
		for (auto& Item : Array)
		{
			Result.Add(Item);
		}
		return Result;
	}

	TArray<PsDataTools::TConstValueType<T, true>> GetCopy() const
	{
		auto& Array = Property->GetValue();
		TArray<PsDataTools::TConstValueType<T, true>> Result;
		Result.Reserve(Array.Num());
		for (auto& Item : Array)
		{
			Result.Add(Item);
		}
		return Result;
	}

	int32 Find(PsDataTools::TConstRefType<T> Element) const
	{
		return Property->GetValue().Find(Element);
	}

	template <typename PredicateType>
	PsDataTools::TConstRefType<T*, bConst> FindByPredicate(const PredicateType& Predicate)
	{
		for (auto It = Property->GetValue().CreateConstIterator(); It; ++It)
		{
			PsDataTools::TConstRefType<T> Item = *It;
			if (Predicate(Item))
			{
				return &(*It);
			}
		}

		return nullptr;
	}

	template <typename PredicateType>
	PsDataTools::TConstRefType<T*, true> FindByPredicate(const PredicateType& Predicate) const
	{
		for (auto It = Property->GetValue().CreateConstIterator(); It; ++It)
		{
			PsDataTools::TConstRefType<T> Item = *It;
			if (Predicate(Item))
			{
				return &(*It);
			}
		}

		return nullptr;
	}

	PsDataTools::TConstRefType<T, bConst> Get(int32 Index)
	{
		return Property->GetValue()[Index];
	}

	PsDataTools::TConstRefType<T, true> Get(int32 Index) const
	{
		return Property->GetValue()[Index];
	}

	int32 Num() const
	{
		return Property->GetValue().Num();
	}

	bool IsEmpty() const
	{
		return Num() == 0;
	}

	bool IsValidIndex(int32 Index) const
	{
		return Property->GetValue().IsValidIndex(Index);
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

	PsDataTools::TConstRefType<T, bConst> operator[](int32 Index)
	{
		return Property->GetValue()[Index];
	}

	PsDataTools::TConstRefType<T, true> operator[](int32 Index) const
	{
		return Property->GetValue()[Index];
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void operator=(const TArray<T>& NewArray)
	{
		Set(NewArray);
	}

	/***********************************
	 * TProxyIterator
	 ***********************************/

public:
	template <bool bIteratorConst>
	struct TProxyIterator
	{
	private:
		PsDataTools::TConstValueType<PsDataTools::TDataProperty<TArray<T>>*, bIteratorConst> Property;
		TArray<PsDataTools::TConstValueType<T, bIteratorConst>> Items;
		int32 Index;

	public:
		explicit TProxyIterator(PsDataTools::TConstValueType<PsDataTools::TDataProperty<TArray<T>>*, bIteratorConst> InProperty, bool bEnd = false)
			: Property(InProperty)
			, Index(0)
		{
			const auto& Array = Property->GetValue();
			if (bEnd)
			{
				Index = Array.Num();
			}
			else
			{
				Items.Append(Array);
			}
		}

		template <bool bOtherConst = bIteratorConst,
			typename = typename TEnableIf<!bOtherConst>::Type>
		int32 RemoveCurrent(bool bAllowShrinking = false)
		{
			int32 CurrentIndex = Property->GetValue().Find(GetValue());
			if (CurrentIndex == INDEX_NONE)
			{
				return INDEX_NONE;
			}

			auto NewArray = Property->GetValue();
			NewArray.RemoveAt(CurrentIndex, 1, bAllowShrinking);
			Property->SetValue(NewArray);

			return Index;
		}

		PsDataTools::TConstRefType<T, bIteratorConst> GetValue() const
		{
			return Items[Index];
		}

		PsDataTools::TConstRefType<T, true> GetConstValue() const
		{
			return Items[Index];
		}

		int32 GetIndex() const
		{
			return Property->GetValue().Find(GetValue());
		}

		TProxyIterator& operator++()
		{
			++Index;
			return *this;
		}

		explicit operator bool() const
		{
			return Index < Items.Num();
		}

		bool operator!() const
		{
			return Index >= Items.Num();
		}

		friend bool operator==(const TProxyIterator& Lhs, const TProxyIterator& Rhs)
		{
			return Lhs.Index == Rhs.Index;
		}

		friend bool operator!=(const TProxyIterator& Lhs, const TProxyIterator& Rhs)
		{
			return Lhs.Index != Rhs.Index;
		}

		const auto& operator*()
		{
			return Items[Index];
		}
	};

	TProxyIterator<bConst> CreateIterator()
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
using TPsDataArrayProxy = TPsDataBaseArrayProxy<T, false>;
template <typename T>
using TPsDataConstArrayProxy = TPsDataBaseArrayProxy<T, true>;

template <typename T, bool bConst>
struct TPsDataBaseArrayProxy<TArray<T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct TPsDataBaseArrayProxy<TMap<FString, T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};

namespace Algo
{
template <typename T, bool bConst, typename PredicateType>
FORCEINLINE PsDataTools::TConstRefType<T*, bConst> FindByPredicate(TPsDataBaseArrayProxy<T, bConst> Proxy, const PredicateType& Predicate)
{
	return Proxy.FindByPredicate(Predicate);
}

template <typename T, typename PredicateType>
FORCEINLINE PsDataTools::TConstRefType<T*, true> FindByPredicate(const TPsDataBaseArrayProxy<T, true> Proxy, const PredicateType& Predicate)
{
	return Proxy.FindByPredicate(Predicate);
}
} // namespace Algo
