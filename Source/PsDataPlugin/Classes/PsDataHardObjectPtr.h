// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsDataTraits.h"

#include "PsDataHardObjectPtr.generated.h"

/***********************************
 * FHardObjectPtrCounter
 ***********************************/

USTRUCT()
struct PSDATAPLUGIN_API FHardObjectPtrCounter
{
	GENERATED_USTRUCT_BODY()

public:
	int32 Num;

	UPROPERTY()
	const UObject* Object;

	FHardObjectPtrCounter();
	FHardObjectPtrCounter(const UObject* InObject);

	friend uint32 GetTypeHash(const FHardObjectPtrCounter& Counter)
	{
		return GetTypeHash(Counter.Object);
	}

	friend bool operator==(const FHardObjectPtrCounter& A, const FHardObjectPtrCounter& B)
	{
		return A.Object == B.Object;
	}
};

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

	UPROPERTY()
	TSet<FHardObjectPtrCounter> List;
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
		Reset();
	}

	void Reset()
	{
		if (Value != nullptr)
		{
			TSet<FHardObjectPtrCounter>& List = UPsDataHardObjectPtrSingleton::Get()->List;

			auto CounterPtr = List.Find(Value);
			check(CounterPtr);

			FHardObjectPtrCounter& Counter = *CounterPtr;
			Counter.Num -= 1;
			check(Counter.Num >= 0);

			if (Counter.Num == 0)
			{
				List.Remove(Counter);
			}
			Value = nullptr;
		}
	}

	void Set(T* NewValue)
	{
		if (Value == NewValue)
		{
			return;
		}

		Reset();

		if (NewValue != nullptr)
		{
			TSet<FHardObjectPtrCounter>& List = UPsDataHardObjectPtrSingleton::Get()->List;
			if (auto CounterPtr = List.Find(NewValue))
			{
				CounterPtr->Num += 1;
			}
			else
			{
				FHardObjectPtrCounter Counter(NewValue);
				Counter.Num += 1;
				List.Add(Counter);
			}
		}
		Value = NewValue;
	}

	T* Get() const
	{
		return Value;
	}

	bool IsValid() const
	{
		return Value->IsValidLowLevel();
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
