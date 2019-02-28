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
 * TConstRef trait
 ***********************************/

namespace FDataReflectionTools
{
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
	typedef T** Type;
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

/***********************************
* Default value trait
***********************************/

template <typename T>
struct FDefaultValue
{
	static T GetDefault() { return T(); }
};

template <>
struct FDefaultValue<int32>
{
	static int32 GetDefault() { return 0; }
};

template <>
struct FDefaultValue<uint8>
{
	static uint8 GetDefault() { return 0; }
};

template <>
struct FDefaultValue<float>
{
	static float GetDefault() { return 0.f; }
};

template <>
struct FDefaultValue<bool>
{
	static bool GetDefault() { return false; }
};
