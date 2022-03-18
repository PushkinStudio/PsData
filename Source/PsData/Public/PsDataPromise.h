// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

#include <utility>

template <typename T>
class PSDATA_API TPsDataPromise
{
public:
	DECLARE_DELEGATE_OneParam(FPsDataPromiseDelegate, const T&);

	TPsDataPromise()
	{
	}

	TPsDataPromise(const T& InValue)
		: Value(InValue)
	{
	}

	TPsDataPromise(T&& InValue)
		: Value(std::move(InValue))
	{
	}

	void Resolve(const T& InValue)
	{
		Value = InValue;
		Execute();
	}

	void Resolve(T&& InValue)
	{
		Value = std::move(InValue);
		Execute();
	}

	void Reset()
	{
		Value.Reset();
	}

	void Bind(const FPsDataPromiseDelegate& Delegate)
	{
		if (Value.IsSet())
		{
			ExecuteSingle(Delegate);
		}
		else
		{
			Delegates.Add(Delegate);
		}
	}

	void Unbind(const FPsDataPromiseDelegate& Delegate)
	{
		for (auto It = Delegates.CreateIterator(); It; ++It)
		{
			if (Delegate.GetHandle() == It->GetHandle())
			{
				It.RemoveCurrent();
			}
		}
	}

private:
	void Execute()
	{
		while (Delegates.Num() > 0)
		{
			TArray<FSimpleDelegate> DelegatesArray = std::move(Delegates);
			for (const auto& Delegate : DelegatesArray)
			{
				ExecuteSingle(Delegate);
			}
		}
	}

	void ExecuteSingle(const FPsDataPromiseDelegate& Delegate)
	{
		Delegate.ExecuteIfBound(Value.Get());
	}

	TOptional<T> Value;
	TArray<FPsDataPromiseDelegate> Delegates;
};

class PSDATA_API FPsDataSimplePromise
{
public:
	FPsDataSimplePromise()
		: bResolved(false)
	{
	}

	FPsDataSimplePromise(bool bInResolved)
		: bResolved(bInResolved)
	{
	}

	void Resolve()
	{
		bResolved = true;
		Execute();
	}

	void Reset()
	{
		bResolved = false;
	}

	void Bind(const FSimpleDelegate& Delegate)
	{
		if (bResolved)
		{
			ExecuteSingle(Delegate);
		}
		else
		{
			Delegates.Add(Delegate);
		}
	}

	void Unbind(const FSimpleDelegate& Delegate)
	{
		for (auto It = Delegates.CreateIterator(); It; ++It)
		{
			if (Delegate.GetHandle() == It->GetHandle())
			{
				It.RemoveCurrent();
			}
		}
	}

private:
	void Execute()
	{
		while (Delegates.Num() > 0)
		{
			TArray<FSimpleDelegate> DelegatesArray = std::move(Delegates);
			for (const auto& Delegate : DelegatesArray)
			{
				ExecuteSingle(Delegate);
			}
		}
	}

	void ExecuteSingle(const FSimpleDelegate& Delegate)
	{
		Delegate.ExecuteIfBound();
	}

	bool bResolved;
	TArray<FSimpleDelegate> Delegates;
};