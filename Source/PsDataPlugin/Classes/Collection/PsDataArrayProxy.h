// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataHardObjectPtr.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"

namespace FDataReflectionTools
{
template <typename T>
struct FArrayChangeBehavior
{
	static void AddToArray(const THardObjectPtr<UPsData>& Instance, int32 Index, const T& Value) {}
	static void RemoveFromArray(const THardObjectPtr<UPsData>& Instance, const T& Value) {}
};

template <typename T>
struct FArrayChangeBehavior<T*>
{
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	static void AddToArray(const THardObjectPtr<UPsData>& Instance, int32 Index, T* Value)
	{
		FPsDataFriend::ChangeDataName(Value, FString::FromInt(Index));
		FPsDataFriend::AddChild(Instance.Get(), Value);
	}

	static void RemoveFromArray(const THardObjectPtr<UPsData>& Instance, T* Value)
	{
		FPsDataFriend::RemoveChild(Instance.Get(), Value);
	}
};
} // namespace FDataReflectionTools

template <typename T, bool bConst>
struct FPsDataBaseArrayProxy
{
private:
	friend struct FPsDataBaseArrayProxy<T, true>;
	friend struct FPsDataBaseArrayProxy<T, false>;

	THardObjectPtr<UPsData> Instance;
	TSharedPtr<const FDataField> Field;

	TArray<T>& Get() const
	{
		check(IsValid());
		TArray<T>* Output = nullptr;
		FDataReflectionTools::GetByField(Instance.Get(), Field, Output);
		check(Output);
		return *Output;
	}

public:
	FPsDataBaseArrayProxy(UPsData* InInstance, TSharedPtr<const FDataField> InField)
		: Instance(InInstance)
		, Field(InField)
	{
		check(IsValid());
	}

	FPsDataBaseArrayProxy(UPsData* InInstance, int32 Hash)
		: Instance(InInstance)
		, Field(FDataReflection::GetFieldByHash(InInstance->GetClass(), Hash))
	{
		check(IsValid());
	}

	template <bool bOtherConst>
	FPsDataBaseArrayProxy(const FPsDataBaseArrayProxy<T, bOtherConst>& ArrayProxy)
		: Instance(ArrayProxy.Instance)
		, Field(ArrayProxy.Field)
	{
		static_assert(bConst == bOtherConst || (bConst && !bOtherConst), "Can't create FPsDataArrayProxy from FPsDataConstArrayProxy");
		check(IsValid());
	}

	template <bool bOtherConst>
	FPsDataBaseArrayProxy(FPsDataBaseArrayProxy<T, bOtherConst>&& ArrayProxy)
		: Instance(std::move(ArrayProxy.Instance))
		, Field(std::move(ArrayProxy.Field))
	{
		static_assert(bConst == bOtherConst || (bConst && !bOtherConst), "Can't create FPsDataArrayProxy from FPsDataConstArrayProxy");
		check(IsValid());
		ArrayProxy.Instance = nullptr;
		ArrayProxy.Field = nullptr;
	}

	FPsDataBaseArrayProxy()
	{
	}

	TSharedPtr<const FDataField> GetField() const
	{
		return Field;
	}

	bool IsValid() const
	{
		return Instance.IsValid() && Field.IsValid();
	}

	int32 Add(typename FDataReflectionTools::TConstRef<T>::Type Element)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstArrayProxy, use FPsDataArrayProxy");

		int32 Index = Get().Add(Element);
		FDataReflectionTools::FArrayChangeBehavior<T>::AddToArray(Instance, Index, Element);
		UPsDataEvent::DispatchChange(Instance.Get(), Field);
		return Index;
	}

	void Insert(typename FDataReflectionTools::TConstRef<T>::Type Element, int32 Index)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstArrayProxy, use FPsDataArrayProxy");

		Get().Insert(Element, Index);
		FDataReflectionTools::FArrayChangeBehavior<T>::AddToArray(Instance, Index, Element);
		UPsDataEvent::DispatchChange(Instance.Get(), Field);
	}

	void RemoveAt(int32 Index)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstArrayProxy, use FPsDataArrayProxy");

		TArray<T>& Array = Get();
		FDataReflectionTools::FArrayChangeBehavior<T>::RemoveFromArray(Instance, Array[Index]);
		Array.RemoveAt(Index);
		UPsDataEvent::DispatchChange(Instance.Get(), Field);
	}

	int32 Remove(typename FDataReflectionTools::TConstRef<T>::Type Element)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstArrayProxy, use FPsDataArrayProxy");

		TArray<T>& Array = Get();
		FDataReflectionTools::FArrayChangeBehavior<T>::RemoveFromArray(Instance, Element);
		int32 Index = Array.Remove(Element);
		UPsDataEvent::DispatchChange(Instance.Get(), Field);
		return Index;
	}

	typename FDataReflectionTools::TConstRef<T, bConst>::Type Get(int32 Index) const
	{
		return Get()[Index];
	}

	int32 Num() const
	{
		return Get().Num();
	}

	void Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
	{
		Instance->BindInternal(Type, Delegate, Field);
	}

	void Bind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		Instance->BindInternal(Type, Delegate, Field);
	}

	void Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
	{
		Instance->UnbindInternal(Type, Delegate, Field);
	}

	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		Instance->UnbindInternal(Type, Delegate, Field);
	}

	const TArray<typename FDataReflectionTools::TConstRef<T, bConst>::Type>& GetRef()
	{
		return Get();
	}

	typename FDataReflectionTools::TConstRef<T, bConst>::Type operator[](int32 Index) const
	{
		return Get()[Index];
	}

	void operator=(const FPsDataBaseArrayProxy& Proxy)
	{
		Instance = Proxy.Instance;
		Field = Proxy.Field;
		check(IsValid());
	}

	/***********************************
	 * TProxyIterator
	 ***********************************/

public:
	struct TProxyIterator
	{
	private:
		friend struct FPsDataBaseArrayProxy;

		FPsDataBaseArrayProxy& Proxy;
		typename TArray<T>::TIterator Iterator;

		TProxyIterator(FPsDataBaseArrayProxy& InProxy)
			: Proxy(InProxy)
			, Iterator(InProxy.Get())
		{
		}

		TProxyIterator(FPsDataBaseArrayProxy& InProxy, bool bEnd)
			: Proxy(InProxy)
			, Iterator(InProxy.Get())
		{
			if (bEnd)
			{
				//TODO: call std::end
				while (Iterator)
				{
					++Iterator;
				}
			}
		}

	public:
		void RemoveCurrent()
		{
			static_assert(!bConst, "Unsupported method for FPsDataConstArrayProxy::TProxyIterator, use FPsDataArrayProxy::TProxyIterator");

			T& Element = *Iterator;
			FDataReflectionTools::FArrayChangeBehavior<T>::RemoveFromArray(Proxy.Instance, Element);
			Iterator.RemoveCurrent();
			UPsDataEvent::DispatchChange(Proxy.Instance.Get(), Proxy.Field);
		}

		TProxyIterator& operator++()
		{
			++Iterator;
			return *this;
		}

		explicit operator bool() const
		{
			return bool(Iterator);
		}

		bool operator!() const
		{
			return !bool(Iterator);
		}

		friend bool operator==(const TProxyIterator& Lhs, const TProxyIterator& Rhs)
		{
			return Lhs.Iterator == Rhs.Iterator;
		}

		friend bool operator!=(const TProxyIterator& Lhs, const TProxyIterator& Rhs)
		{
			return Lhs.Iterator != Rhs.Iterator;
		}

		typename FDataReflectionTools::TConstRef<T, bConst>::Type operator*() const
		{
			return *Iterator;
		}
	};

	TProxyIterator CreateIterator()
	{
		return TProxyIterator(*this);
	}

	TProxyIterator begin() { return TProxyIterator(*this); }
	TProxyIterator end() { return TProxyIterator(*this, true); }
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
