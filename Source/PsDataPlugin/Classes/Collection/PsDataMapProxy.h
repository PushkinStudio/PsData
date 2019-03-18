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
struct FMapChangeBehavior
{
	static void AddToMap(const THardObjectPtr<UPsData>& Instance, const FString& Name, const T& Value) {}
	static void RemoveFromMap(const THardObjectPtr<UPsData>& Instance, const T& Value) {}
};

template <typename T>
struct FMapChangeBehavior<T*>
{
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	static void AddToMap(const THardObjectPtr<UPsData>& Instance, const FString& Name, T* Value)
	{
		FPsDataFriend::ChangeDataName(Value, Name);
		FPsDataFriend::AddChild(Instance.Get(), Value);
	}

	static void RemoveFromMap(const THardObjectPtr<UPsData>& Instance, T* Value)
	{
		FPsDataFriend::RemoveChild(Instance.Get(), Value);
	}
};
} // namespace FDataReflectionTools

template <typename T, bool bConst>
struct FPsDataBaseMapProxy
{
private:
	friend struct FPsDataBaseMapProxy<T, true>;
	friend struct FPsDataBaseMapProxy<T, false>;

	THardObjectPtr<UPsData> Instance;
	TSharedPtr<const FDataField> Field;

	TMap<FString, T>& Get() const
	{
		check(IsValid());
		TMap<FString, T>* Output = nullptr;
		FDataReflectionTools::GetByField(Instance.Get(), Field, Output);
		check(Output);
		return *Output;
	}

public:
	FPsDataBaseMapProxy(UPsData* InInstance, TSharedPtr<const FDataField> InField)
		: Instance(InInstance)
		, Field(InField)
	{
		check(IsValid());
	}

	FPsDataBaseMapProxy(UPsData* InInstance, int32 Hash)
		: Instance(InInstance)
		, Field(FDataReflection::GetFieldByHash(InInstance->GetClass(), Hash))
	{
		check(IsValid());
	}

	template <bool bOtherConst>
	FPsDataBaseMapProxy(const FPsDataBaseMapProxy<T, bOtherConst>& MapProxy)
		: Instance(MapProxy.Instance)
		, Field(MapProxy.Field)
	{
		static_assert(bConst == bOtherConst || (bConst && !bOtherConst), "Can't create FPsDataMapProxy from FPsDataConstMapProxy");
		check(IsValid());
	}

	template <bool bOtherConst>
	FPsDataBaseMapProxy(FPsDataBaseMapProxy<T, bOtherConst>&& MapProxy)
		: Instance(std::move(MapProxy.Instance))
		, Field(std::move(MapProxy.Field))
	{
		static_assert(bConst == bOtherConst || (bConst && !bOtherConst), "Can't create FPsDataMapProxy from FPsDataConstMapProxy");
		check(IsValid());
		MapProxy.Instance = nullptr;
		MapProxy.Field = nullptr;
	}

	FPsDataBaseMapProxy()
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

	bool Add(const FString& Key, typename FDataReflectionTools::TConstRef<T>::Type Element)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstMapProxy, use FPsDataMapProxy");

		Get().Add(Key, Element);
		FDataReflectionTools::FMapChangeBehavior<T>::AddToMap(Instance, Key, Element);
		UPsDataEvent::DispatchChange(Instance.Get(), Field);
		return true;
	}

	bool Remove(const FString& Key)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstMapProxy, use FPsDataMapProxy");

		TMap<FString, T>& Map = Get();
		T* ElementPtr = Map.Find(Key);
		if (ElementPtr)
		{
			T& Element = *ElementPtr;
			FDataReflectionTools::FMapChangeBehavior<T>::RemoveFromMap(Instance, Element);
			Map.Remove(Key);
			UPsDataEvent::DispatchChange(Instance.Get(), Field);
			return true;
		}
		return false;
	}

	void Clear()
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstMapProxy, use FPsDataMapProxy");

		TArray<FString> Keys;
		Get().GetKeys(Keys);
		for (auto& Key : Keys)
		{
			Remove(Key);
		}
	}

	bool Contains(const FString& Key) const
	{
		return Get().Contains(Key);
	}

	typename FDataReflectionTools::TConstRef<T*, bConst>::Type Find(const FString& Key) const
	{
		return Get().Find(Key);
	}

	typename FDataReflectionTools::TConstRef<T, bConst>::Type FindChecked(const FString& Key) const
	{
		return Get().FindChecked(Key);
	}

	TArray<FString> GetKeys() const
	{
		TArray<FString> Keys;
		Get().GetKeys(Keys);
		return Keys;
	}

	TArray<typename FDataReflectionTools::TConstRef<T, bConst>::Type> GetValues() const
	{
		TArray<typename FDataReflectionTools::TConstRef<T, bConst>::Type> Result;
		for (auto It = Get().CreateIterator(); It; ++It)
		{
			Result.Add((*It).Value);
		}
		return Result;
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

	const TMap<FString, typename FDataReflectionTools::TConstRef<T, bConst>::Type>& GetRef()
	{
		return Get();
	}

	typename FDataReflectionTools::TConstRef<T, bConst>::Type operator[](const FString& Key) const
	{
		return Get().FindChecked(Key);
	}

	void operator=(const FPsDataBaseMapProxy& Proxy)
	{
		Instance = Proxy.Instance;
		Field = Proxy.Field;
		check(IsValid());
	}

	/***********************************
     * TProxyIterator
     ***********************************/

public:
	struct TProxyPair
	{
	private:
		friend struct TProxyIterator;

		FString& PairKey;
		T& PairValue;

	public:
		TProxyPair(FString& InKey, T& InValue)
			: PairKey(InKey)
			, PairValue(InValue)
		{
		}

		const FString& Key() const { return PairKey; }
		typename FDataReflectionTools::TConstRef<T, bConst>::Type Value() const { return PairValue; }
	};

	struct TProxyIterator
	{
	private:
		friend struct FPsDataBaseMapProxy;

		FPsDataBaseMapProxy& Proxy;
		typename TMap<FString, T>::TIterator Iterator;

		TProxyIterator(FPsDataBaseMapProxy& InProxy)
			: Proxy(InProxy)
			, Iterator(InProxy.Get())
		{
		}

		TProxyIterator(FPsDataBaseMapProxy& InProxy, bool bEnd)
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
			static_assert(!bConst, "Unsupported method for FPsDataConstMapProxy::TProxyIterator, use FPsDataMapProxy::TProxyIterator");

			T& Element = Iterator->Value();
			FDataReflectionTools::FMapChangeBehavior<T>::RemoveFromMap(Proxy.Instance, Element);
			UPsDataEvent::DispatchChange(Proxy.Instance.Get(), Proxy.Field);
			Iterator.RemoveCurrent();
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

		const FString& Key() const
		{
			return Iterator->Key;
		}

		typename FDataReflectionTools::TConstRef<T, bConst>::Type Value() const
		{
			return Iterator->Value;
		}

		TProxyPair operator*() const
		{
			return TProxyPair(Iterator->Key, Iterator->Value);
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
using FPsDataMapProxy = FPsDataBaseMapProxy<T, false>;
template <class T>
using FPsDataConstMapProxy = FPsDataBaseMapProxy<T, true>;

template <typename T, bool bConst>
struct FPsDataBaseMapProxy<TArray<T>, bConst>
{
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct FPsDataBaseMapProxy<TMap<FString, T>, bConst>
{
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};
