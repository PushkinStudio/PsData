// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataProperty.h"

#include "CoreMinimal.h"

namespace PsDataTools
{

/***********************************
 * TDataLinkType
 ***********************************/

template <typename DataKeyType, typename DataValueType>
struct TDataLinkType
{
	using KeyType = DataKeyType;
	using ValueType = DataValueType;
	using LinkKeyType = FString;
	using LinkValueType = TConstValueType<DataValueType>;
	using LinkValueContextType = DataValueType;
};

template <typename DataKeyType, typename DataValueType>
struct TDataLinkType<TArray<DataKeyType>, DataValueType>
{
	using KeyType = DataKeyType;
	using ValueType = DataValueType;
	using LinkKeyType = TArray<FString>;
	using LinkValueType = TArray<TConstValueType<DataValueType>>;
	using LinkValueContextType = TArray<DataValueType>;
};

template <typename DataKeyType, typename DataValueType>
struct TDataLinkType<TMap<FString, DataKeyType>, DataValueType>
{
	static_assert(TAlwaysFalse<DataKeyType>::value, "Unsupported type");
};

template <typename LinkKeyType, typename LinkValueType>
struct TDataLinkCache
{
	TDataLinkCache()
		: bValidKey(false)
		, bValidValue(false)
	{
	}

	void Reset()
	{
		bValidKey = false;
		bValidValue = false;
		BindCollection.Unbind();
	}

	bool bValidKey;
	bool bValidValue;

	FString Path;
	LinkKeyType Key;
	LinkValueType Value;
	FPsDataBindCollection BindCollection;
};

template <typename LinkValueContextType>
struct TDataLinkPropertyGetter : public FAbstractDataLinkProperty
{
	virtual LinkValueContextType& GetValueRef() const = 0;
};

template <typename DataKeyType, typename DataValueType>
struct TAbstractDataLinkProperty : public TDataLinkPropertyGetter<typename TDataLinkType<DataKeyType, DataValueType>::LinkValueContextType>
{
	static_assert(TIsContainer<DataValueType>::Value, "Return type must be non-container type");

	using Types = TDataLinkType<DataKeyType, DataValueType>;

	TAbstractDataLinkProperty()
	{
	}

	virtual ~TAbstractDataLinkProperty() override
	{
	}

	void Construct()
	{
		ResetDelegate = FPsDataDelegate::CreateLambda([this](UPsDataEvent* Event) {
			Cache.Reset();
		});

		const auto Owner = this->GetOwner();
		const auto Link = this->GetLink();

		BindCollection.Add(Owner->Bind(UPsDataEvent::RemovedFromRoot, ResetDelegate, EDataBindFlags::NonDeferred));
		BindCollection.Add(Owner->Bind(Link->Field->GetChangedEventName(), ResetDelegate, EDataBindFlags::IgnoreFieldMeta | EDataBindFlags::NonDeferred));
	}

	void Destruct()
	{
		Cache.Reset();
		ResetDelegate.Unbind();
		BindCollection.Unbind();
	}

protected:
	virtual typename Types::LinkValueContextType& GetValueRef() const override
	{
		UpdateValue();

		check(Cache.bValidValue);
		return Cache.Value;
	}

	typename Types::LinkValueType GetValue() const
	{
		return CastInternal(GetValueRef());
	}

	bool IsEmptyKey() const
	{
		const auto Link = this->GetLink();
		if (!Link->Meta.bNullable)
		{
			return false;
		}

		UpdateKey();

		check(Cache.bValidKey);
		return IsEmptyKeyInternal(Cache.Key);
	}

private:
	void UpdatePath() const
	{
		const auto Owner = this->GetOwner();
		const auto Link = this->GetLink();

		check(Link->PathFunction != nullptr);

		const auto PreviousPath = Cache.Path;
		Link->PathFunction(Owner, Cache.Path);
		if (PreviousPath != Cache.Path)
		{
			Cache.Reset();
		}
	}

	virtual bool UpdateKeyInternal(UPsData* Owner, const FDataLink* Link, typename Types::LinkKeyType& OutKey) const = 0;
	virtual bool IsEmptyKeyInternal(const typename Types::LinkKeyType& Key) const = 0;

	void UpdateKey() const
	{
		UpdatePath();

		const auto Owner = this->GetOwner();
		const auto Link = this->GetLink();

		if (!Cache.bValidKey)
		{
			UpdateKeyInternal(Owner, Link, Cache.Key);

			if (!Link->Meta.bNullable && IsEmptyKeyInternal(Cache.Key))
			{
				UE_LOG(LogData, Fatal, TEXT("Link %s::%s without Nullable meta can't be empty"), *Owner->GetClass()->GetName(), *Link->Field->Name);
			}

			Cache.bValidKey = true;
		}
	}

	virtual bool UpdateValueInternal(UPsData* Owner, const FDataLink* Link, const FDataPathExecutor& Executor, const typename Types::LinkKeyType& Key, typename Types::LinkValueContextType& OutValue, FPsDataBindCollection& InOutBindCollection, const FPsDataDelegate& Delegate) const = 0;

	void UpdateValue() const
	{
		UpdateKey();

		if (!Cache.bValidValue)
		{
			check(Cache.bValidKey);

			const auto Owner = this->GetOwner();
			const auto Link = this->GetLink();

			check(Owner->HasRoot());

			FDataPathExecutor Executor = FDataPathExecutor(Owner->GetRoot(), Cache.Path);
			const auto bExecuteSuccess = Executor.Execute();
			check(bExecuteSuccess);

			const auto ExecutorData = Executor.GetData();
			const auto ExecutorField = Executor.GetField();

			Cache.BindCollection.Add(ExecutorData->Bind(UPsDataEvent::RemovedFromRoot, ResetDelegate, EDataBindFlags::NonDeferred));
			Cache.BindCollection.Add(ExecutorData->Bind(ExecutorField->GetChangedEventName(), ResetDelegate, EDataBindFlags::IgnoreFieldMeta | EDataBindFlags::NonDeferred));

			if (!UpdateValueInternal(Owner, Link, Executor, Cache.Key, Cache.Value, Cache.BindCollection, ResetDelegate))
			{
				if (!Link->Meta.bNullable)
				{
					UE_LOG(LogData, Fatal, TEXT("Link %s::%s (path: %s) without Nullable meta can't be null"), *Owner->GetClass()->GetName(), *Link->Field->Name, *Cache.Path);
				}
			}

			Cache.bValidValue = true;
		}
	}

	virtual void ValidateInternal(UPsData* Owner, const FDataLink* Link, const FString& Path, const TDataPathExecutor<false, false>& Executor, const typename Types::LinkKeyType& Key, TArray<FString>& OutResult) const = 0;

	virtual void Validate(TArray<FString>& OutResult) const override
	{
		const auto Owner = this->GetOwner();
		const auto Link = this->GetLink();

		if (!Owner->HasRoot())
		{
			OutResult.Add(FString::Printf(TEXT("Link %s::%s doesn't have root data"), *Owner->GetClass()->GetName(), *Link->Field->Name));
			return;
		}

		FString LinkPath;
		Link->PathFunction(Owner, LinkPath);
		if (LinkPath.Len() == 0)
		{
			OutResult.Add(FString::Printf(TEXT("Link %s::%s has empty path"), *Owner->GetClass()->GetName(), *Link->Field->Name));
			return;
		}

		auto Executor = TDataPathExecutor<false, false>(Owner->GetRoot(), LinkPath);
		if (!Executor.Execute())
		{
			OutResult.Add(FString::Printf(TEXT("Link %s::%s has broken path: %s"), *Owner->GetClass()->GetName(), *Link->Field->Name, *LinkPath));
			return;
		}

		typename Types::LinkKeyType Key;
		UpdateKeyInternal(Owner, Link, Key);

		if (IsEmptyKeyInternal(Key))
		{
			if (!Link->Meta.bNullable)
			{
				OutResult.Add(FString::Printf(TEXT("Link %s::%s has empty key without Nullable meta"), *Owner->GetClass()->GetName(), *Link->Field->Name));
			}
			return;
		}

		ValidateInternal(Owner, Link, LinkPath, Executor, Key, OutResult);
	}

	virtual typename Types::LinkValueType CastInternal(const typename Types::LinkValueContextType& InValue) const = 0;

	mutable TDataLinkCache<typename Types::LinkKeyType, typename Types::LinkValueContextType> Cache;
	FPsDataDelegate ResetDelegate;
	FPsDataBindCollection BindCollection;
};

/***********************************
 * TDataLinkProperty
 ***********************************/

template <typename DataKeyType, typename DataValueType>
struct TDataLinkProperty : public TAbstractDataLinkProperty<DataKeyType, DataValueType>
{
	TDataLinkProperty()
	{
	}

	virtual ~TDataLinkProperty() override {}

private:
	virtual bool UpdateKeyInternal(UPsData* Owner, const FDataLink* Link, FString& OutKey) const override
	{
		DataKeyType* KeyPtr = nullptr;
		const bool bSuccess = GetByField<false>(Owner, Link->Field, KeyPtr);
		check(bSuccess);

		OutKey = TTypeToString<DataKeyType>::ToString(*KeyPtr);
		return true;
	}

	virtual bool IsEmptyKeyInternal(const FString& Key) const override
	{
		return Key.Len() == 0;
	}

	virtual bool UpdateValueInternal(UPsData* Owner, const FDataLink* Link, const FDataPathExecutor& Executor, const FString& Key, DataValueType& OutValue, FPsDataBindCollection& InOutBindCollection, const FPsDataDelegate& Delegate) const override
	{
		TDataPathExecutor<false, false> KeyExecutor = Executor;
		KeyExecutor.AppendKey(Key);

		DataValueType* ValuePtr = nullptr;
		if (KeyExecutor.Execute(ValuePtr))
		{
			if (Executor.GetData() != KeyExecutor.GetData())
			{
				InOutBindCollection.Add(KeyExecutor.GetData()->Bind(UPsDataEvent::RemovedFromRoot, Delegate, EDataBindFlags::NonDeferred));
				InOutBindCollection.Add(KeyExecutor.GetData()->Bind(KeyExecutor.GetField()->GetChangedEventName(), Delegate, EDataBindFlags::IgnoreFieldMeta | EDataBindFlags::NonDeferred));
			}

			OutValue = *ValuePtr;
			return true;
		}
		else
		{
			OutValue = TTypeDefault<DataValueType>::GetDefaultValue();
			return false;
		}
	}

	virtual void ValidateInternal(UPsData* Owner, const FDataLink* Link, const FString& Path, const TDataPathExecutor<false, false>& Executor, const FString& Key, TArray<FString>& OutResult) const override
	{
		TDataPathExecutor<false, false> KeyExecutor = Executor;
		KeyExecutor.AppendKey(Key);

		DataValueType* ValuePtr = nullptr;
		if (!KeyExecutor.Execute(ValuePtr))
		{
			OutResult.Add(FString::Printf(TEXT("Link %s::%s has invalid key: %s (%s.%s)"), *Owner->GetClass()->GetName(), *Link->Field->Name, *Key, *Path, *Key));
		}
	}

	virtual TConstValueType<DataValueType> CastInternal(const DataValueType& Value) const override
	{
		return Value;
	}
};

/***********************************
 * TDataLinkProperty for Array
 ***********************************/

template <typename DataKeyType, typename DataValueType>
struct TDataLinkProperty<TArray<DataKeyType>, DataValueType> : public TAbstractDataLinkProperty<TArray<DataKeyType>, DataValueType>
{
	TDataLinkProperty()
	{
	}

	virtual ~TDataLinkProperty() override {}

private:
	virtual bool UpdateKeyInternal(UPsData* Owner, const FDataLink* Link, TArray<FString>& OutKeys) const override
	{
		OutKeys.Reset();

		TArray<DataKeyType>* ArrayPtr = nullptr;
		const bool bSuccess = GetByField<false>(Owner, Link->Field, ArrayPtr);
		check(bSuccess);

		OutKeys = TTypeToString<TArray<DataKeyType>>::ToString(*ArrayPtr);
		return true;
	}

	virtual bool IsEmptyKeyInternal(const TArray<FString>& Keys) const override
	{
		for (const auto& Key : Keys)
		{
			if (Key.Len() == 0)
			{
				return true;
			}
		}

		return false;
	}

	virtual bool UpdateValueInternal(UPsData* Owner, const FDataLink* Link, const FDataPathExecutor& Executor, const TArray<FString>& Keys, TArray<DataValueType>& OutValues, FPsDataBindCollection& InOutBindCollection, const FPsDataDelegate& Delegate) const override
	{
		OutValues.Reset();

		int32 NullCounter = false;
		for (const auto& Key : Keys)
		{
			TDataPathExecutor<false, false> KeyExecutor = Executor;
			KeyExecutor.AppendKey(Key);

			DataValueType* ValuePtr = nullptr;
			if (KeyExecutor.Execute(ValuePtr))
			{
				if (Executor.GetData() != KeyExecutor.GetData())
				{
					InOutBindCollection.Add(KeyExecutor.GetData()->Bind(UPsDataEvent::RemovedFromRoot, Delegate, EDataBindFlags::NonDeferred));
					InOutBindCollection.Add(KeyExecutor.GetData()->Bind(KeyExecutor.GetField()->GetChangedEventName(), Delegate, EDataBindFlags::IgnoreFieldMeta | EDataBindFlags::NonDeferred));
				}

				OutValues.Add(*ValuePtr);
			}
			else
			{
				OutValues.Add(TTypeDefault<DataValueType>::GetDefaultValue());
				NullCounter += 1;
			}
		}

		return NullCounter == 0;
	}

	virtual void ValidateInternal(UPsData* Owner, const FDataLink* Link, const FString& Path, const TDataPathExecutor<false, false>& Executor, const TArray<FString>& Keys, TArray<FString>& OutResult) const override
	{
		for (const auto& Key : Keys)
		{
			TDataPathExecutor<false, false> KeyExecutor = Executor;
			KeyExecutor.AppendKey(Key);

			DataValueType* ValuePtr = nullptr;
			if (!KeyExecutor.Execute(ValuePtr))
			{
				OutResult.Add(FString::Printf(TEXT("Link %s::%s has invalid key: %s (%s.%s)"), *Owner->GetClass()->GetName(), *Link->Field->Name, *Key, *Path, *Key));
			}
		}
	}

	virtual TArray<TConstValueType<DataValueType>> CastInternal(const TArray<DataValueType>& Values) const override
	{
		TArray<TConstValueType<DataValueType>> Result;
		Result.Append(Values);
		return Result;
	}
};

/***********************************
 * Unsafe get link
 ***********************************/

template <typename T>
bool UnsafeGetLinkValueByIndex(UPsData* Instance, int32 Index, T*& OutValue)
{
	TDataLinkPropertyGetter<T>* LinkProperty = static_cast<TDataLinkPropertyGetter<T>*>(FPsDataFriend::GetLinks(Instance)[Index]);
	OutValue = &LinkProperty->GetValueRef();
	return true;
}

template <typename T>
bool UnsafeGetLinkValue(UPsData* Instance, const FDataLink* Link, T*& OutValue)
{
	return UnsafeGetByIndex<T>(Instance, Link->Index, OutValue);
}

/***********************************
 * Get link
 ***********************************/

template <bool bThrowError, typename T>
bool GetLinkValue(UPsData* Instance, const FDataLink* Link, T*& OutValue)
{
	if (Instance && Link)
	{
		auto OutputContext = &GetContext<T>();
		auto ReturnContext = Link->ReturnContext;
		if (CheckType<T>(OutputContext, ReturnContext) && UnsafeGetLinkValue(Instance, Link, OutValue))
		{
			return true;
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't cast link %s::%s from %s to %s"), *Instance->GetClass()->GetName(), *Link->Field->Name, *ReturnContext->GetCppType(), *OutputContext->GetCppType());
		}
	}
	else
	{
		if (bThrowError && !Instance)
		{
			UE_LOG(LogData, Fatal, TEXT("Instance is null"));
		}

		if (bThrowError && !Link)
		{
			UE_LOG(LogData, Fatal, TEXT("Link is null"));
		}
	}

	return false;
}

} // namespace PsDataTools
