// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataHardObjectPtr.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"

template <typename T, bool bConst>
struct FPsDataBaseArrayProxy
{
private:
	friend struct FPsDataBaseArrayProxy<T, true>;
	friend struct FPsDataBaseArrayProxy<T, false>;

	THardObjectPtr<UPsData> Instance;
	FDataProperty<TArray<T>>* Property;

	static FDataProperty<TArray<T>>* GetProperty(UPsData* Instance, const TSharedPtr<const FDataField>& Field)
	{
		TArray<T>* Output = nullptr;
		FDataReflectionTools::GetByField(Instance, Field, Output);
		check(Output);

		return static_cast<FDataProperty<TArray<T>>*>(FDataReflectionTools::FPsDataFriend::GetProperties(Instance)[Field->Index]);
	}

protected:
	friend class UPsDataBlueprintArrayProxy;

	FPsDataBaseArrayProxy()
	{
	}

public:
	FPsDataBaseArrayProxy(UPsData* InInstance, const TSharedPtr<const FDataField>& InField)
		: Instance(InInstance)
		, Property(GetProperty(InInstance, InField))
	{
		check(IsValid());
	}

	FPsDataBaseArrayProxy(UPsData* InInstance, int32 Hash)
		: Instance(InInstance)
		, Property(GetProperty(InInstance, FDataReflection::GetFieldByHash(InInstance->GetClass(), Hash)))
	{
		check(IsValid());
	}

	FPsDataBaseArrayProxy(UPsData* InInstance, const FDataProperty<TArray<T>>* InProperty)
		: Instance(InInstance)
		, Property(const_cast<FDataProperty<TArray<T>>*>(InProperty)) // TODO: const_cast
	{
		check(IsValid());
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseArrayProxy(const FPsDataBaseArrayProxy<T, bOtherConst>& ArrayProxy)
		: Instance(ArrayProxy.Instance)
		, Property(ArrayProxy.Property)
	{
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	FPsDataBaseArrayProxy(FPsDataBaseArrayProxy<T, bOtherConst>&& ArrayProxy)
		: Instance(std::move(ArrayProxy.Instance))
		, Property(ArrayProxy.Property)
	{
		ArrayProxy.Instance = nullptr;
		ArrayProxy.Property = nullptr;
	}

	template <bool bOtherConst,
		typename = typename TEnableIf<bConst == bOtherConst || (bConst && !bOtherConst)>::Type>
	void operator=(const FPsDataBaseArrayProxy<T, bOtherConst>& ArrayProxy)
	{
		Instance = ArrayProxy.Instance;
		Property = ArrayProxy.Property;
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
	void Set(const TArray<T>& NewArray)
	{
		Property->Set(NewArray, Instance.Get());
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	int32 Add(typename FDataReflectionTools::TConstRef<T, false>::Type Element)
	{
		auto NewArray = Property->Get();
		auto Index = NewArray.Add(Element);
		Property->Set(NewArray, Instance.Get());

		return Index;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void Insert(typename FDataReflectionTools::TConstRef<T, false>::Type Element, int32 Index)
	{
		auto NewArray = Property->Get();
		NewArray.Insert(Element, Index);
		Property->Set(NewArray, Instance.Get());
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	void RemoveAt(int32 Index, bool bAllowShrinking = false)
	{
		auto NewArray = Property->Get();
		NewArray.RemoveAt(Index, 1, bAllowShrinking);
		Property->Set(NewArray, Instance.Get());
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	int32 Remove(typename FDataReflectionTools::TConstRef<T, false>::Type Element, bool bAllowShrinking = false)
	{
		auto Index = Property->Get().Find(Element);
		if (Index == INDEX_NONE)
		{
			return INDEX_NONE;
		}

		auto NewArray = Property->Get();
		NewArray.RemoveAt(Index, 1, bAllowShrinking);
		Property->Set(NewArray, Instance.Get());

		return Index;
	}

	template <typename PredicateType, bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	int32 RemoveAll(const PredicateType& Predicate)
	{
		auto NewArray = Property->Get();

		int32 RemovedElements = 0;
		for (auto It = NewArray.CreateIterator(); It; ++It)
		{
			typename FDataReflectionTools::TConstRef<T, true>::Type Item = *It;
			if (Predicate(Item))
			{
				It.RemoveCurrent();
				++RemovedElements;
			}
		}

		Property->Set(NewArray, Instance.Get());
		return RemovedElements;
	}

	template <bool bOtherConst = bConst,
		typename = typename TEnableIf<!bOtherConst>::Type>
	typename FDataReflectionTools::TConstRef<T, bConst>::Type Set(typename FDataReflectionTools::TConstRef<T, false>::Type Element, int32 Index)
	{
		auto NewArray = Property->Get();
		auto& OldElement = NewArray[Index];
		NewArray[Index] = Element;
		Property->Set(NewArray, Instance.Get());
		return OldElement;
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

	typename FDataReflectionTools::TConstValue<TArray<T>, bConst>::Type Get() const
	{
		auto& Array = Property->Get();
		typename FDataReflectionTools::TConstValue<TArray<T>, bConst>::Type Result;
		Result.Reserve(Array.Num());
		for (auto& Item : Array)
		{
			Result.Add(Item);
		}
		return Result;
	}

	int32 Find(typename FDataReflectionTools::TConstRef<T>::Type Element) const
	{
		return Property->Get().Find(Element);
	}

	template <typename PredicateType>
	typename FDataReflectionTools::TConstRef<T*, bConst>::Type FindByPredicate(const PredicateType& Predicate) const
	{
		for (auto It = Property->Get().CreateIterator(); It; ++It)
		{
			typename FDataReflectionTools::TConstRef<T, true>::Type Item = *It;
			if (Predicate(Item))
			{
				return &(*It);
			}
		}

		return nullptr;
	}

	typename FDataReflectionTools::TConstRef<T, bConst>::Type Get(int32 Index) const
	{
		return Property->Get()[Index];
	}

	int32 Num() const
	{
		return Property->Get().Num();
	}

	bool IsEmpty() const
	{
		return Num() == 0;
	}

	bool IsValidIndex(int32 Index) const
	{
		return Property->Get().IsValidIndex(Index);
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

	typename FDataReflectionTools::TConstRef<T, bConst>::Type operator[](int32 Index) const
	{
		return Property->Get()[Index];
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
			const auto& Array = Proxy.Property->Get();
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

		typename FDataReflectionTools::TConstRef<T, bIteratorConst>::Type GetValue() const
		{
			return Items[Index];
		}

		typename FDataReflectionTools::TConstRef<T, true>::Type GetConstValue() const
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

		typename FDataReflectionTools::TConstRef<T, bIteratorConst>::Type operator*() const
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
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct FPsDataBaseArrayProxy<TMap<FString, T>, bConst>
{
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};
