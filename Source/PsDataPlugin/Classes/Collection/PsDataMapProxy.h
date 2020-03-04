// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

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
	static void Add(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, const T& Value, TFunction<void()> AddAction)
	{
		AddAction();
		FPsDataFriend::Changed(Instance, Field);
	}

	static void Remove(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, const T& Value, TFunction<void()> RemoveAction)
	{
		RemoveAction();
		FPsDataFriend::Changed(Instance, Field);
	}

	static void Replace(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, const T& OldValue, const T& NewValue, TFunction<void()> ReplaceAction)
	{
		ReplaceAction();
		FPsDataFriend::Changed(Instance, Field);
	}
};

template <typename T>
struct FMapChangeBehavior<T*>
{
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	static void Add(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, T* Value, TFunction<void()> AddAction)
	{
		AddAction();
		FPsDataFriend::ChangeDataName(Value, Name, Field->Name);
		FPsDataFriend::AddChild(Instance, Value);
		FPsDataFriend::Changed(Instance, Field);
	}

	static void Remove(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, T* Value, TFunction<void()> RemoveAction)
	{
		RemoveAction();
		FPsDataFriend::RemoveChild(Instance, Value);
		FPsDataFriend::Changed(Instance, Field);
	}

	static void Replace(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, T* OldValue, T* NewValue, TFunction<void()> ReplaceAction)
	{
		ReplaceAction();
		FPsDataFriend::RemoveChild(Instance, OldValue);
		FPsDataFriend::ChangeDataName(NewValue, Name, Field->Name);
		FPsDataFriend::AddChild(Instance, NewValue);
		FPsDataFriend::Changed(Instance, Field);
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
		FDataReflectionTools::GetByField(*Instance, Field, Output);
		check(Output);
		return *Output;
	}

protected:
	friend class UPsDataBlueprintMapProxy;

	FPsDataBaseMapProxy()
	{
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

		auto& Map = Get();
		if (auto Find = Map.Find(Key))
		{
			FDataReflectionTools::FMapChangeBehavior<T>::Replace(Instance.Get(), Field, Key, *Find, Element, [&Map, &Key, &Element]() {
				Map.Add(Key, Element);
			});
		}
		else
		{
			FDataReflectionTools::FMapChangeBehavior<T>::Add(Instance.Get(), Field, Key, Element, [&Map, &Key, &Element]() {
				Map.Add(Key, Element);
			});
		}
		return true;
	}

	bool Remove(const FString& Key)
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstMapProxy, use FPsDataMapProxy");

		auto& Map = Get();
		if (auto Find = Map.Find(Key))
		{
			FDataReflectionTools::FMapChangeBehavior<T>::Remove(Instance.Get(), Field, Key, *Find, [&Map, &Key]() {
				Map.Remove(Key);
			});

			return true;
		}

		return false;
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
		auto& Map = Get();
		TArray<FString> Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Key);
		}
		return Result;
	}

	TArray<typename FDataReflectionTools::TConstRef<T, bConst>::Type> GetValues() const
	{
		auto& Map = Get();
		TArray<typename FDataReflectionTools::TConstRef<T, bConst>::Type> Result;
		Result.Reserve(Map.Num());
		for (auto& Pair : Map)
		{
			Result.Add(Pair.Value);
		}
		return Result;
	}

	int32 Num() const
	{
		return Get().Num();
	}

	void Reserve(int32 Number)
	{
		Get().Reserve(Number);
	}

	void Empty()
	{
		static_assert(!bConst, "Unsupported method for FPsDataConstMapProxy, use FPsDataMapProxy");

		for (auto It = CreateIterator(); It; ++It)
		{
			It.RemoveCurrent();
		}
	}

	bool IsEmpty() const
	{
		return Num() == 0;
	}

	FPsDataBind Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
	{
		return Instance->BindInternal(Type, Delegate, Field);
	}

	FPsDataBind Bind(const FString& Type, const FPsDataDelegate& Delegate) const
	{
		return Instance->BindInternal(Type, Delegate, Field);
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
	template <bool bIteratorConst>
	struct TProxyIterator
	{
		typedef typename TMap<FString, typename FDataReflectionTools::TConstValue<T, bIteratorConst>::Type>::ElementType PairType;

	private:
		friend struct FPsDataBaseMapProxy;

		typedef typename FDataReflectionTools::TConstRef<FPsDataBaseMapProxy, bIteratorConst>::Type ProxyType;
		typedef typename FDataReflectionTools::TSelector<typename TMap<FString, T>::TConstIterator, typename TMap<FString, T>::TIterator, bIteratorConst>::Value IteratorType;

		ProxyType& Proxy;
		IteratorType Iterator;

		TProxyIterator(ProxyType& InProxy, bool bEnd = false)
			: Proxy(InProxy)
			, Iterator(InProxy.Get())
		{
			if (bEnd)
			{
				while (Iterator) //TODO: call std::end
				{
					++Iterator;
				}
			}
		}

	public:
		void RemoveCurrent()
		{
			static_assert(!bIteratorConst, "Unsupported method for FPsDataConstMapProxy::TProxyIterator, use FPsDataMapProxy::TProxyIterator");

			FDataReflectionTools::FMapChangeBehavior<T>::Remove(Proxy.Instance.Get(), Proxy.Field, Iterator.Key(), Iterator.Value(), [this]() {
				Iterator.RemoveCurrent();
			});
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

		typename FDataReflectionTools::TConstRef<T, bIteratorConst>::Type Value() const
		{
			return Iterator->Value;
		}

		const PairType& operator*() const
		{
			return *reinterpret_cast<const PairType*>(&(*Iterator));
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
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct FPsDataBaseMapProxy<TMap<FString, T>, bConst>
{
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};
