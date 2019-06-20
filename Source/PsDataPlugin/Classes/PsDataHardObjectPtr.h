// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/EnableIf.h"
#include "Templates/PointerIsConvertibleFromTo.h"
#include "Templates/SharedPointerInternals.h"

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
};

template <class T>
class THardObjectPtr
{
private:
	T* Value;

public:
	THardObjectPtr(SharedPointerInternals::FNullTag* = nullptr)
		: Value(nullptr)
	{
	}

	template <
		typename OtherType,
		typename = typename TEnableIf<TPointerIsConvertibleFromTo<OtherType, T>::Value>::Type>
	THardObjectPtr(OtherType* NewValue)
		: Value(nullptr)
	{
		Set(NewValue);
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
			// Sometimes we don't know type in destructor. We can trust "set" method to verify T
			ResetInternal(static_cast<const UObject*>(static_cast<const void*>(Value))); // TODO: see std::~unique_ptr
			Value = nullptr;
		}
	}

	template <class OtherType>
	friend class THardObjectPtr;

private:
	static void ResetInternal(const UObject* Object)
	{
		if (GExitPurge || IsIncrementalPurgePending())
		{
			return;
		}

		if (Object != nullptr)
		{
			auto& ObjectCounters = UPsDataHardObjectPtrSingleton::Get()->ObjectCounters;
			auto& Count = ObjectCounters.FindChecked(Object);
			check(Count > 0);
			--Count;
			if (Count == 0)
			{
				ObjectCounters.Remove(Object);
			}
		}
	}

	static void SetInternal(const UObject* Object)
	{
		if (Object != nullptr)
		{
			auto& ObjectCounters = UPsDataHardObjectPtrSingleton::Get()->ObjectCounters;
			if (!ObjectCounters.Contains(Object))
			{
				ObjectCounters.Add(Object, 0);
			}

			auto& Count = ObjectCounters.FindChecked(Object);
			check(Count >= 0);
			Count++;
		}
	}

public:
	void Reset()
	{
		ResetInternal(Value);
		Value = nullptr;
	}

	void Set(T* NewValue)
	{
		if (Value == NewValue)
		{
			return;
		}

		ResetInternal(Value);
		SetInternal(NewValue);
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
