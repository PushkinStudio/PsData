// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/EnableIf.h"
#include "Templates/PointerIsConvertibleFromTo.h"

#include "PsDataHardObjectPtr.generated.h"

/***********************************
 * UPsDataHardObjectPtrSingleton
 ***********************************/

UCLASS()
class PSDATAPLUGIN_API UPsDataHardObjectPtrSingleton : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	static UPsDataHardObjectPtrSingleton* Singleton;

public:
	static void Init();

protected:
	template <class T>
	friend class THardObjectPtr;

	static UPsDataHardObjectPtrSingleton* Get();

	UPROPERTY(Transient)
	TMap<const UObject*, int32> ObjectCounters;

	static void RetainObject(const UObject* Object);
	static void ReleaseObject(const UObject* Object);
};

template <class T>
class THardObjectPtr
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

	~THardObjectPtr()
	{
		if (Value != nullptr)
		{
			// In the case of a forward declaration we don't know complete type of T and need to use ResetUnsafe
			ResetUnsafe();
		}
	}

	template <class OtherType>
	friend class THardObjectPtr;

public:
	void Reset()
	{
		ResetImpl(Value);
	}

	void ResetUnsafe()
	{
		// Implementation of the Set method guarantees that this cast is safety
		const auto Object = static_cast<const UObject*>(static_cast<const void*>(Value));
		ResetImpl(Object);
	}

	void ResetImpl(const UObject* Object)
	{
		UPsDataHardObjectPtrSingleton::ReleaseObject(Object);
		Value = nullptr;
	}

	void Set(T* NewValue)
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UObject>::Value, "Only UObjects are allowed");

		if (Value == NewValue)
		{
			return;
		}

		if (Value != nullptr)
		{
			Reset();
		}

		if (NewValue != nullptr)
		{
			UPsDataHardObjectPtrSingleton::RetainObject(NewValue);
			Value = NewValue;
		}
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
