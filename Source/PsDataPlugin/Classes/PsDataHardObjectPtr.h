// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsDataTraits.h"

#include "PsDataHardObjectPtr.generated.h"

/***********************************
 * UPsDataSingleton
 ***********************************/

UCLASS()
class PSDATAPLUGIN_API UPsDataHardObjectPtrSingleton : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	template <class T>
	friend struct THardObjectPtr;

	static UPsDataHardObjectPtrSingleton* Singleton;

	static UPsDataHardObjectPtrSingleton* Get();

	// @TODO ZEN-770 UPROPERTY()
	TMap<const UObject*, int32> ObjectCounters;
};

template <class T>
struct THardObjectPtr
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

	~THardObjectPtr()
	{
		if (Value != nullptr)
		{
			// Sometimes we don't know type in destructor. We can trust "set" method to verify T
			ResetInternal(static_cast<const UObject*>(static_cast<const void*>(Value))); // TODO: see std::~unique_ptr
			Value = nullptr;
		}
	}

private:
	static void ResetInternal(const UObject* Object)
	{
		if (GExitPurge)
		{
			return;
		}

		if (Object != nullptr)
		{
			auto& Count = UPsDataHardObjectPtrSingleton::Get()->ObjectCounters.FindChecked(Object);
			Count--;

			check(Count >= 0);
			if (Count == 0)
			{
				UPsDataHardObjectPtrSingleton::Get()->ObjectCounters.Remove(Object);
				const_cast<UObject*>(Object)->ClearFlags(RF_StrongRefOnFrame);
			}
		}
	}

	static void SetInternal(const UObject* Object)
	{
		if (Object != nullptr)
		{
			auto& Count = UPsDataHardObjectPtrSingleton::Get()->ObjectCounters.FindOrAdd(Object);
			check(Count >= 0);
			Count++;

			// Add to root on first usage
			if (Count == 1)
			{
				// @TODO ZEN-770 Check cluster add instead of root one
				const_cast<UObject*>(Object)->SetFlags(RF_StrongRefOnFrame);
			}
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
		return Get();
	}

	T* operator->() const
	{
		return Get();
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
