// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PsDataStringView.h"

#include <type_traits>

namespace PsDataTools
{

/***********************************
 * TAlwaysFalse trait
 ***********************************/

template <typename T>
struct TAlwaysFalse : std::false_type
{
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
	using Value = True;
};

template <typename True, typename False>
struct TSelector<True, False, false>
{
	using Value = False;
};

/***********************************
 * TConstValue trait
 ***********************************/

template <typename T, bool bConst>
struct TConstValue
{
	using Type = T;
};

template <typename T, bool bConst>
struct TConstValue<T*, bConst>
{
	using Type = typename TSelector<T const*, T*, bConst>::Value;
};

template <typename T, bool bConst>
struct TConstValue<T**, bConst>
{
	using Type = typename TSelector<T const* const*, T**, bConst>::Value;
};

template <typename T, bool bConst = true>
using TConstValueType = typename TConstValue<T, bConst>::Type;

/***********************************
 * TConstRef trait
 ***********************************/

template <typename T, bool bConst>
struct TConstRef
{
	using Type = const T&;
};

template <typename T, bool bConst>
struct TConstRef<T*, bConst>
{
	using Type = typename TSelector<T const*, T*, bConst>::Value;
};

template <typename T, bool bConst>
struct TConstRef<T**, bConst>
{
	using Type = typename TSelector<T const* const*, T* const*, bConst>::Value;
};

template <typename T, bool bConst = true>
using TConstRefType = typename TConstRef<T, bConst>::Type;

/***********************************
 * Is collection trait
 ***********************************/

template <typename T>
struct TIsContainer
{
	static constexpr bool Value = true;
	static constexpr bool Array = false;
	static constexpr bool Map = false;
	using Type = T;
};

template <typename T>
struct TIsContainer<TArray<T>>
{
	static constexpr bool Value = false;
	static constexpr bool Array = true;
	static constexpr bool Map = false;
	using Type = T;
};

template <typename K, typename T>
struct TIsContainer<TMap<K, T>>
{
	static constexpr bool Value = false;
	static constexpr bool Array = false;
	static constexpr bool Map = true;
	using Type = T;
};

#if defined(__clang__)
#define __FUNCTION_SIGNATURE__ __PRETTY_FUNCTION__
#else
#define __FUNCTION_SIGNATURE__ __FUNCSIG__
#endif

constexpr FDataStringViewChar GetSignature(const char* s)
{
	const FDataStringViewChar Prefix("FType<");
	const FDataStringViewChar Postfix(">::ContentType");
	FDataStringViewChar Signature(s);

	Signature.RightChopInline(Signature.Find(Prefix) + Prefix.Len());
	Signature.LeftInline(Signature.Find(Postfix));

	return Signature;
}

template <typename T>
struct FType
{
	static FString ContentType()
	{
		constexpr FDataStringViewChar Signature = GetSignature(__FUNCTION_SIGNATURE__);
		FString Result = FString(Signature.Len(), Signature.GetData());

		Result.ReplaceInline(TEXT("> "), TEXT(">"));

#if !defined(__clang__)
		Result.RemoveFromStart(TEXT("enum "), ESearchCase::CaseSensitive);
		Result.RemoveFromStart(TEXT("class "), ESearchCase::CaseSensitive);
		Result.RemoveFromStart(TEXT("struct "), ESearchCase::CaseSensitive);
#endif

		return Result;
	}

	static FString Type()
	{
		return ContentType();
	}

	static constexpr uint32 Hash()
	{
		return FDataStringViewChar(__FUNCTION_SIGNATURE__).GetHash();
	}
};

template <typename T>
struct FType<T*>
{
	static FString ContentType()
	{
		return FType<T>::ContentType();
	}

	static FString Type()
	{
		return FString::Printf(TEXT("%s*"), *ContentType());
	}

	static constexpr uint32 Hash()
	{
		return FDataStringViewChar(__FUNCTION_SIGNATURE__).GetHash();
	}
};

template <typename T>
struct FType<TArray<T>>
{
	static FString ContentType()
	{
		return FType<typename TRemovePointer<T>::Type>::ContentType();
	}

	static FString Type()
	{
		return FString::Printf(TEXT("TArray<%s>"), *FType<T>::Type());
	}

	static constexpr uint32 Hash()
	{
		return FDataStringViewChar(__FUNCTION_SIGNATURE__).GetHash();
	}
};

template <typename T>
struct FType<TMap<FString, T>>
{
	static FString ContentType()
	{
		return FType<typename TRemovePointer<T>::Type>::ContentType();
	}

	static FString Type()
	{
		return FString::Printf(TEXT("TMap<FString, %s>"), *FType<T>::Type());
	}

	static constexpr uint32 Hash()
	{
		return FDataStringViewChar(__FUNCTION_SIGNATURE__).GetHash();
	}
};

} // namespace PsDataTools
