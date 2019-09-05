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
 * TConstValue trait
 ***********************************/

namespace FDataReflectionTools
{
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
} // namespace FDataReflectionTools

/***********************************
* Is collection trait
***********************************/

namespace FDataReflectionTools
{
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

} // namespace FDataReflectionTools
