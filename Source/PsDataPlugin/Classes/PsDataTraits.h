// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

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

} // namespace FDataReflectionTools

/***********************************
 * TRemovePointer trait
 ***********************************/

namespace FDataReflectionTools
{
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

} // namespace FDataReflectionTools

/***********************************
 * TSelector trait
 ***********************************/

namespace FDataReflectionTools
{
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
} // namespace FDataReflectionTools

/***********************************
 * TConstRef trait
 ***********************************/

namespace FDataReflectionTools
{
template <typename T, bool bConst = false>
struct TConstRef
{
	typedef typename TSelector<T, const T&, std::is_arithmetic<T>::value>::Value Type;
};

template <typename T>
struct TConstRef<const T>
{
	typedef typename TSelector<const T, const T&, std::is_arithmetic<T>::value>::Value Type;
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

} // namespace FDataReflectionTools

/***********************************
* Is collection trait
***********************************/

namespace FDataReflectionTools
{
template <typename T>
struct TIsContainer
{
	static const std::true_type Value;
	static const std::false_type Array;
	static const std::false_type Map;
};

template <typename T>
struct TIsContainer<TArray<T>>
{
	static const std::false_type Value;
	static const std::true_type Array;
	static const std::false_type Map;
};

template <typename K, typename T>
struct TIsContainer<TMap<K, T>>
{
	static const std::false_type Value;
	static const std::false_type Array;
	static const std::true_type Map;
};

} // namespace FDataReflectionTools
