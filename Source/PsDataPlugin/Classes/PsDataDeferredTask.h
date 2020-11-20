// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Async/Async.h"
#include "CoreMinimal.h"

namespace PsDataTools
{

struct PSDATAPLUGIN_API FDeferredUniqueTaskHelper
{
	static bool Register(void* Owner, const FGuid& FunctionGuid);
	static void Unregister(void* Owner, const FGuid& FunctionGuid);

private:
	static TMap<void*, TSet<FGuid>> Registered;
};

// For UObjects

template <typename T, typename R, typename... ArgTypes, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, R (T::*MemberFunction)(ArgTypes...), ArgTypes&&... Args)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid() && FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, FunctionGuid, MemberFunction, WeakOwner] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			if (WeakOwner.IsValid())
			{
				(Owner->*MemberFunction)(Forward<ArgTypes>(Args)...);
			}
		});

		return true;
	}

	return false;
}

template <typename T, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, TFunction<void()> Function)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid() && FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, FunctionGuid, Function, WeakOwner] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			if (WeakOwner.IsValid())
			{
				Function();
			}
		});

		return true;
	}

	return false;
}

template <typename T, typename R, typename... ArgTypes, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, R (T::*MemberFunction)(ArgTypes...), ArgTypes&&... Args)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid())
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, MemberFunction, WeakOwner] {
			if (WeakOwner.IsValid())
			{
				(Owner->*MemberFunction)(Forward<ArgTypes>(Args)...);
			}
		});
	}
}

template <typename T, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, TFunction<void()> Function)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid())
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, Function, WeakOwner] {
			if (WeakOwner.IsValid())
			{
				Function();
			}
		});
	}
}

// For any types

template <typename T, typename R, typename... ArgTypes, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, R (T::*MemberFunction)(ArgTypes...), ArgTypes&&... Args)
{
	if (FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, FunctionGuid, MemberFunction] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			(Owner->*MemberFunction)(Forward<ArgTypes>(Args)...);
		});

		return true;
	}

	return false;
}

template <typename T, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, TFunction<void()> Function)
{
	if (FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, FunctionGuid, Function] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			Function();
		});

		return true;
	}

	return false;
}

template <typename T, typename R, typename... ArgTypes, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, R (T::*MemberFunction)(ArgTypes...), ArgTypes&&... Args)
{
	AsyncTask(ENamedThreads::GameThread, [Owner, MemberFunction] {
		(Owner->*MemberFunction)(Forward<ArgTypes>(Args)...);
	});
}

template <typename T, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, TFunction<void()> Function)
{
	AsyncTask(ENamedThreads::GameThread, [Owner, Function] {
		Function();
	});
}

} // namespace PsDataTools