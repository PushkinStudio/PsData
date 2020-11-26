// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/EnableIf.h"
#include "Templates/PointerIsConvertibleFromTo.h"
#include "UObject/GCObject.h"
#include "UObject/UObjectGlobals.h"

template <class T>
class THardObjectPtr : public FGCObject
{
private:
	T* Value;

public:
	THardObjectPtr()
		: Value(nullptr)
	{
	}

	THardObjectPtr(T* NewValue)
		: Value(nullptr)
	{
		Set(NewValue);
	}

	THardObjectPtr(const THardObjectPtr& Other)
		: Value(nullptr)
	{
		Set(Other.Value);
	}

	THardObjectPtr(THardObjectPtr&& Other)
		: Value(Other.Value)
	{
		Other.Value = nullptr;
	}

	template <
		typename OtherType,
		typename = typename TEnableIf<TPointerIsConvertibleFromTo<OtherType, T>::Value>::Type>
	THardObjectPtr(const THardObjectPtr<OtherType>& Other)
		: Value(nullptr)
	{
		Set(Other.Value);
	}

	template <
		typename OtherType,
		typename = typename TEnableIf<TPointerIsConvertibleFromTo<OtherType, T>::Value>::Type>
	THardObjectPtr(THardObjectPtr<OtherType>&& Other)
		: Value(Other.Value)
	{
		Other.Value = nullptr;
	}

	template <class OtherType>
	friend class THardObjectPtr;

public:
	void Reset()
	{
		Value = nullptr;
	}

	void Set(T* NewValue)
	{
		Value = NewValue;
	}

	T* Get() const
	{
		return Value;
	}

	bool IsValid() const
	{
		return Value != nullptr && Value->IsValidLowLevel();
	}

	operator bool() const
	{
		return IsValid();
	}

	bool operator!() const
	{
		return !IsValid();
	}

	T* operator*() const
	{
		return Value;
	}

	T* operator->() const
	{
		return Value;
	}

	void operator=(T* NewValue)
	{
		Set(NewValue);
	}

	void operator=(const THardObjectPtr& NewPtr)
	{
		Set(NewPtr.Value);
	}

private:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(Value);
	}

	virtual FString GetReferencerName() const override
	{
		return "THardObjectPtr";
	}
};

template <typename LhsT, typename RhsT>
bool operator==(const THardObjectPtr<LhsT>& Lhs, const THardObjectPtr<RhsT>& Rhs)
{
	return Lhs.Get() == Rhs.Get();
}

template <typename LhsT, typename RhsT>
bool operator==(const THardObjectPtr<LhsT>& Lhs, const RhsT* Rhs)
{
	return Lhs.Get() == Rhs;
}

template <typename LhsT, typename RhsT>
bool operator==(const LhsT* Lhs, const THardObjectPtr<RhsT>& Rhs)
{
	return Lhs == Rhs.Get();
}

template <typename LhsT>
bool operator==(const THardObjectPtr<LhsT>& Lhs, TYPE_OF_NULLPTR)
{
	return !Lhs.IsValid();
}

template <typename RhsT>
bool operator==(TYPE_OF_NULLPTR, const THardObjectPtr<RhsT>& Rhs)
{
	return !Rhs.IsValid();
}

template <typename LhsT, typename RhsT>
bool operator!=(const THardObjectPtr<LhsT>& Lhs, const THardObjectPtr<RhsT>& Rhs)
{
	return Lhs.Get() != Rhs.Get();
}

template <typename LhsT, typename RhsT>
bool operator!=(const THardObjectPtr<LhsT>& Lhs, const RhsT* Rhs)
{
	return Lhs.Get() != Rhs;
}

template <typename LhsT, typename RhsT>
bool operator!=(const LhsT* Lhs, const THardObjectPtr<RhsT>& Rhs)
{
	return Lhs != Rhs.Get();
}

template <typename LhsT>
bool operator!=(const THardObjectPtr<LhsT>& Lhs, TYPE_OF_NULLPTR)
{
	return Lhs.IsValid();
}

template <typename RhsT>
bool operator!=(TYPE_OF_NULLPTR, const THardObjectPtr<RhsT>& Rhs)
{
	return Rhs.IsValid();
}
