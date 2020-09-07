// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataHardObjectPtr.h"
#include "PsDataTraits.h"
#include "Types/PsData_UPsData.h"

#include "CoreMinimal.h"

namespace FDataReflectionTools
{
template <typename T>
struct FMapChangeBehavior
{
	static void Add(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, const T& Value, TFunction<void()> AddAction, bool bDispacthChanged = true)
	{
		AddAction();
		if (bDispacthChanged)
		{
			FPsDataFriend::Changed(Instance, Field);
		}
	}

	static void Remove(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, const T& Value, TFunction<void()> RemoveAction, bool bDispacthChanged = true)
	{
		RemoveAction();
		if (bDispacthChanged)
		{
			FPsDataFriend::Changed(Instance, Field);
		}
	}

	static void Replace(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, const T& OldValue, const T& NewValue, TFunction<void()> ReplaceAction, bool bDispacthChanged = true)
	{
		ReplaceAction();
		if (bDispacthChanged)
		{
			FPsDataFriend::Changed(Instance, Field);
		}
	}
};

template <typename T>
struct FMapChangeBehavior<T*>
{
	static void Add(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, T* Value, TFunction<void()> AddAction, bool bDispacthChanged = true)
	{
		AddAction();
		FPsDataFriend::ChangeDataName(Value, Name, Field->Name);
		FPsDataFriend::AddChild(Instance, Value);
		if (bDispacthChanged)
		{
			FPsDataFriend::Changed(Instance, Field);
		}
	}

	static void Remove(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, T* Value, TFunction<void()> RemoveAction, bool bDispacthChanged = true)
	{
		RemoveAction();
		FPsDataFriend::RemoveChild(Instance, Value);
		if (bDispacthChanged)
		{
			FPsDataFriend::Changed(Instance, Field);
		}
	}

	static void Replace(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Name, T* OldValue, T* NewValue, TFunction<void()> ReplaceAction, bool bDispacthChanged = true)
	{
		ReplaceAction();
		FPsDataFriend::RemoveChild(Instance, OldValue);
		FPsDataFriend::ChangeDataName(NewValue, Name, Field->Name);
		FPsDataFriend::AddChild(Instance, NewValue);
		if (bDispacthChanged)
		{
			FPsDataFriend::Changed(Instance, Field);
		}
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

		bool bChanged = false;
		for (auto It = Get().CreateIterator(); It; ++It)
		{
			FDataReflectionTools::FMapChangeBehavior<T>::Remove(
				Instance.Get(), Field, It->Key, It->Value, [&It, &bChanged]() {
					It.RemoveCurrent();
					bChanged = true;
				},
				false);
		}

		if (bChanged)
		{
			FDataReflectionTools::FPsDataFriend::Changed(Instance.Get(), Field);
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
		struct TPair
		{
			const FString Key;
			typename FDataReflectionTools::TConstValue<T, bIteratorConst>::Type Value;

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
			const auto& Map = Proxy.Get();
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
		bool RemoveCurrent()
		{
			static_assert(!bIteratorConst, "Unsupported method for FPsDataConstMapProxy::TProxyIterator, use FPsDataMapProxy::TProxyIterator");

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

		typename FDataReflectionTools::TConstRef<T, bIteratorConst>::Type Value() const
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
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, bool bConst>
struct FPsDataBaseMapProxy<TMap<FString, T>, bConst>
{
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};
