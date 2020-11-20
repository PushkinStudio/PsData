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

// Apply a tuple with the calculated number of arguments for non-const member function
template <typename T, typename R, typename... ArgTypes, typename TupleType, size_t... S>
R ApplyTupleImpl(T* Owner, R (T::*MemberFunction)(ArgTypes...), TupleType&& Tuple, std::index_sequence<S...>)
{
	return (Owner->*MemberFunction)(std::get<S>(std::forward<TupleType>(Tuple))...);
}

// Apply a tuple for non-const member function
template <typename T, typename R, typename... ArgTypes, typename TupleType>
R ApplyTuple(T* Owner, R (T::*MemberFunction)(ArgTypes...), TupleType&& Tuple)
{
	constexpr std::size_t Size = std::tuple_size<typename std::remove_reference<TupleType>::type>::value;
	return ApplyTupleImpl(Owner, MemberFunction, Tuple, std::make_index_sequence<Size>());
}

// Apply a tuple with the calculated number of arguments for const member function
template <typename T, typename R, typename... ArgTypes, typename TupleType, size_t... S>
R ApplyTupleImpl(T* Owner, R (T::*MemberFunction)(ArgTypes...) const, TupleType&& Tuple, std::index_sequence<S...>)
{
	return (Owner->*MemberFunction)(std::get<S>(std::forward<TupleType>(Tuple))...);
}

// Apply a tuple for const member function
template <typename T, typename R, typename... ArgTypes, typename TupleType>
R ApplyTuple(T* Owner, R (T::*MemberFunction)(ArgTypes...) const, TupleType&& Tuple)
{
	constexpr std::size_t Size = std::tuple_size<typename std::remove_reference<TupleType>::type>::value;
	return ApplyTupleImpl(Owner, MemberFunction, Tuple, std::make_index_sequence<Size>());
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, R (T::*MemberFunction)(ArgTypes...), ParamTypes&&... Args)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid() && FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, WeakOwner, FunctionGuid, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			if (WeakOwner.IsValid())
			{
				ApplyTuple(Owner, MemberFunction, std::move(Tuple));
			}
		});

		return true;
	}

	return false;
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, R (T::*MemberFunction)(ArgTypes...) const, ParamTypes&&... Args)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid() && FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, WeakOwner, FunctionGuid, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			if (WeakOwner.IsValid())
			{
				ApplyTuple(Owner, MemberFunction, std::move(Tuple));
			}
		});

		return true;
	}

	return false;
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, R (T::*MemberFunction)(ArgTypes...), ParamTypes&&... Args)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid())
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, WeakOwner, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
			if (WeakOwner.IsValid())
			{
				ApplyTuple(Owner, MemberFunction, std::move(Tuple));
			}
		});
	}
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, R (T::*MemberFunction)(ArgTypes...) const, ParamTypes&&... Args)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid())
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, WeakOwner, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
			if (WeakOwner.IsValid())
			{
				ApplyTuple(Owner, MemberFunction, std::move(Tuple));
			}
		});
	}
}

template <typename T, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, TFunction<void()> Function)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid() && FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, WeakOwner, FunctionGuid, Function] {
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

template <typename T, typename TEnableIf<TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, TFunction<void()> Function)
{
	auto WeakOwner = MakeWeakObjectPtr(Owner);
	if (WeakOwner.IsValid())
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, WeakOwner, Function] {
			if (WeakOwner.IsValid())
			{
				Function();
			}
		});
	}
}

// For any types

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, R (T::*MemberFunction)(ArgTypes...), ParamTypes&&... Args)
{
	if (FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, FunctionGuid, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			ApplyTuple(Owner, MemberFunction, std::move(Tuple));
		});

		return true;
	}

	return false;
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
bool DeferredUniqueTask(T* Owner, const FGuid& FunctionGuid, R (T::*MemberFunction)(ArgTypes...) const, ParamTypes&&... Args)
{
	if (FDeferredUniqueTaskHelper::Register(Owner, FunctionGuid))
	{
		AsyncTask(ENamedThreads::GameThread, [Owner, FunctionGuid, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
			FDeferredUniqueTaskHelper::Unregister(Owner, FunctionGuid);
			ApplyTuple(Owner, MemberFunction, std::move(Tuple));
		});

		return true;
	}

	return false;
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, R (T::*MemberFunction)(ArgTypes...), ParamTypes&&... Args)
{
	AsyncTask(ENamedThreads::GameThread, [Owner, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
		ApplyTuple(Owner, MemberFunction, std::move(Tuple));
	});
}

template <typename T, typename R, typename... ArgTypes, typename... ParamTypes, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, R (T::*MemberFunction)(ArgTypes...) const, ParamTypes&&... Args)
{
	AsyncTask(ENamedThreads::GameThread, [Owner, MemberFunction, Tuple = std::make_tuple(std::forward<ParamTypes>(Args)...)] {
		ApplyTuple(Owner, MemberFunction, std::move(Tuple));
	});
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

template <typename T, typename TEnableIf<!TPointerIsConvertibleFromTo<T, UObject>::Value, int>::Type = 0>
void DeferredTask(T* Owner, TFunction<void()> Function)
{
	AsyncTask(ENamedThreads::GameThread, [Owner, Function] {
		Function();
	});
}

} // namespace PsDataTools