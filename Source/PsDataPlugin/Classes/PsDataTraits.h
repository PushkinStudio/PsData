// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include <type_traits>

/***********************************
 * TAlwaysFalse trait
 ***********************************/

namespace FDataReflectionTools
{
template <typename T>
struct TAlwaysFalse : std::false_type
{
};

/***********************************
 * TRemovePointer trait
 ***********************************/

template <typename T>
struct TRemovePointer
{
	typedef T Type;
	static T& Get(T& Value) { return Value; }
};

template <typename T>
struct TRemovePointer<T*>
{
	typedef T Type;
	static T& Get(T*& Value) { return *Value; }
};

/***********************************
 * TSelector trait
 ***********************************/

template <typename True, typename False, bool bValue>
struct TSelector
{
};

template <typename True, typename False>
struct TSelector<True, False, true>
{
	typedef True Value;
};

template <typename True, typename False>
struct TSelector<True, False, false>
{
	typedef False Value;
};

/***********************************
 * TConstRef trait
 ***********************************/

template <typename T, bool bConst = false>
struct TConstRef
{
	typedef const T& Type;
};

template <typename T>
struct TConstRef<const T>
{
	typedef const T& Type;
};

template <typename T>
struct TConstRef<T&>
{
	typedef const T& Type;
};

template <typename T>
struct TConstRef<const T&>
{
	typedef const T& Type;
};

template <typename T>
struct TConstRef<T*, false>
{
	typedef T* Type;
};

template <typename T>
struct TConstRef<T*, true>
{
	typedef const T* Type;
};

template <typename T>
struct TConstRef<T**, false>
{
	typedef T* const* Type;
};

template <typename T>
struct TConstRef<T**, true>
{
	typedef const T* const* Type;
};

/***********************************
 * TConstValue trait
 ***********************************/

template <typename T, bool bConst>
struct TConstValue
{
	typedef typename TSelector<const T, T, bConst>::Value Type;
};

template <typename T, bool bConst>
struct TConstValue<T*, bConst>
{
	typedef typename TSelector<const T* const, T*, bConst>::Value Type;
};

template <typename T, bool bConst>
struct TConstValue<TArray<T>, bConst>
{
	typedef TArray<T> Type;
};

template <typename T, bool bConst>
struct TConstValue<TArray<T*>, bConst>
{
	typedef typename TSelector<TArray<T const*>, TArray<T*>, bConst>::Value Type;
};

template <typename T, bool bConst, typename F>
struct TConstValue<TMap<F, T>, bConst>
{
	typedef TMap<F, T> Type;
};

template <typename T, bool bConst, typename F>
struct TConstValue<TMap<F, T*>, bConst>
{
	typedef typename TSelector<TMap<F, T const*>, TMap<F, T*>, bConst>::Value Type;
};

/***********************************
* Is collection trait
***********************************/

template <typename T>
struct TIsContainer
{
	static constexpr bool Value = true;
	static constexpr bool Array = false;
	static constexpr bool Map = false;
};

template <typename T>
struct TIsContainer<TArray<T>>
{
	static constexpr bool Value = false;
	static constexpr bool Array = true;
	static constexpr bool Map = false;
};

template <typename K, typename T>
struct TIsContainer<TMap<K, T>>
{
	static constexpr bool Value = false;
	static constexpr bool Array = false;
	static constexpr bool Map = true;
};

/***********************************
* Const cast
***********************************/

template <typename T>
T* MutableThis(const T* This)
{
	return const_cast<T*>(This);
}

} // namespace FDataReflectionTools
