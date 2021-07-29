// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

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
		Delegates.Add(Delegate);

		if (Value.IsSet())
		{
			ExecuteSingle(Delegate);
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
		auto Copy = Delegates;
		for (const auto& Delegate : Copy)
		{
			BroadcastSingle(Delegate);
		}
	}

	void ExecuteSingle(const FPsDataPromiseDelegate& Delegate)
	{
		Delegate.ExecuteIfBound(Value.Get());
	}

	TOptional<T> Value;
	TArray<FPsDataPromiseDelegate> Delegates;
};

class PSDATA_API TPsDataSimplePromise
{
public:
	TPsDataSimplePromise()
		: bResolved(false)
	{
	}

	TPsDataSimplePromise(bool bInResolved)
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
		Delegates.Add(Delegate);

		if (bResolved)
		{
			ExecuteSingle(Delegate);
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
		auto Copy = Delegates;
		for (const auto& Delegate : Copy)
		{
			ExecuteSingle(Delegate);
		}
	}

	void ExecuteSingle(const FSimpleDelegate& Delegate)
	{
		Delegate.ExecuteIfBound();
	}

	bool bResolved;
	TArray<FSimpleDelegate> Delegates;
};