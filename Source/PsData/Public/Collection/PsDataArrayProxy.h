// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataField.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"

template <typename T, bool bConst>
struct FPsDataBaseArrayProxy
{
private:
	friend struct FPsDataBaseArrayProxy<T, true>;
	friend struct FPsDataBaseArrayProxy<T, false>;

	PsDataTools::TConstValueType<PsDataTools::TDataProperty<TArray<T>>*, bConst> Property;

	static PsDataTools::TDataProperty<TArray<T>>* GetProperty(UPsData* Instance, const FDataField* Field)
	{
#if !UE_BUILD_SHIPPING
		TArray<T>* Output = nullptr;
		PsDataTools::GetByField<true>(Instance, Field, Output);
#endif

		return static_cast<PsDataTools::TDataProperty<TArray<T>>*>(PsDataTools::FPsDataFriend::GetProperty(Instance, Field->Index));
	}

	static const PsDataTools::TDataProperty<TArray<T>>* GetProperty(const UPsData* Instance, const FDataField* Field)
	{
#if !UE_BUILD_SHIPPING
		TArray<T>* Output = nullptr;
		PsDataTools::GetByField<true>(Instance, Field, Output);
#endif

		return static_cast<const PsDataTools::TDataProperty<TArray<T>>*>(PsDataTools::FPsDataFriend::GetProperty(Instance, Field->Index));
	}

protected:
	friend class UPsDataBlueprintArrayProxy;

	FPsDataBaseArrayProxy()
		: Property(nullptr)
	{
	}

public:
	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	FPsDataBaseArrayProxy(UPsData* InInstance, const FDataField* InField)
		: Property(GetProperty(InInstance, InField))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	FPsDataBaseArrayProxy(UPsData* InInstance, int32 Hash)
		: Property(GetProperty(InInstance, PsDataTools::FDataReflection::GetFieldsByClass(InInstance->GetClass())->GetFieldByHashChecked(Hash)))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	FPsDataBaseArrayProxy(PsDataTools::TDataProperty<TArray<T>>* InProperty)
		: Property(InProperty)
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<bOtherConst>::Type>
	FPsDataBaseArrayProxy(const UPsData* InInstance, const FDataField* InField)
		: Property(GetProperty(InInstance, InField))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<bOtherConst>::Type>
	FPsDataBaseArrayProxy(const UPsData* InInstance, int32 Hash)
		: Property(GetProperty(InInstance, PsDataTools::FDataReflection::GetFieldsByClass(InInstance->GetClass())->GetFieldByHashChecked(Hash)))
	{
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<bOtherConst>::Type>
	FPsDataBaseArrayProxy(const PsDataTools::TDataProperty<TArray<T>>* InProperty)
		: Property(InProperty)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseArrayProxy(const FPsDataBaseArrayProxy<T, bOtherConst>& ArrayProxy)
		: Property(ArrayProxy.Property)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseArrayProxy(FPsDataBaseArrayProxy<T, bOtherConst>&& ArrayProxy)
		: Property(ArrayProxy.Property)
	{
		ArrayProxy.Property = nullptr;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const FPsDataBaseArrayProxy<T, bOtherConst>& ArrayProxy)
	{
		Property = ArrayProxy.Property;
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

	auto Get() const
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

	int32 Find(PsDataTools::TConstRefType<T> Element) const
	{
		return Property->GetValue().Find(Element);
	}

	template <typename PredicateType>
	PsDataTools::TConstRefType<T*, bConst> FindByPredicate(const PredicateType& Predicate) const
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

	PsDataTools::TConstRefType<T, bConst> Get(int32 Index) const
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
		PsDataTools::FPsDataFriend::UnbindInternal(Property->GetOwner(), Type, Delegate, Property->GetField());
	}

	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		PsDataTools::FPsDataFriend::UnbindInternal(Property->GetOwner(), Type, Delegate, Property->GetField());
	}

	PsDataTools::TConstRefType<T, bConst> operator[](int32 Index) const
	{
		return Property->GetValue()[Index];
	}

	/***********************************
	 * TProxyIterator
	 ***********************************/

public:
	template <bool bIteratorConst>
	struct TProxyIterator
	{
	private:
		friend struct FPsDataBaseArrayProxy;

		FPsDataBaseArrayProxy<T, bIteratorConst> Proxy;
		TArray<T> Items;
		int32 Index;

		TProxyIterator(const FPsDataBaseArrayProxy<T, bIteratorConst>& InProxy, bool bEnd = false)
			: Proxy(InProxy)
			, Index(0)
		{
			const auto& Array = Proxy.Property->GetValue();
			if (bEnd)
			{
				Index = Array.Num();
			}
			else
			{
				Items = Array;
			}
		}

	public:
		template <bool bOtherConst = bIteratorConst,
			typename = typename TEnableIf<!bOtherConst>::Type>
		int32 RemoveCurrent()
		{
			return Proxy.Remove(GetValue());
		}

		PsDataTools::TConstRefType<T, bIteratorConst> GetValue() const
		{
			return Items[Index];
		}

		PsDataTools::TConstRefType<T> GetConstValue() const
		{
			return GetValue();
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

		PsDataTools::TConstRefType<T, bIteratorConst> operator*() const
		{
			return Items[Index];
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
using FPsDataArrayProxy = FPsDataBaseArrayProxy<T, false>;
template <class T>
using FPsDataConstArrayProxy = FPsDataBaseArrayProxy<T, true>;

template <typename T, bool bConst>
struct FPsDataBaseArrayProxy<TArray<T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct FPsDataBaseArrayProxy<TMap<FString, T>, bConst>
{
	static_assert(PsDataTools::TAlwaysFalse<T>::value, "Unsupported type");
};
