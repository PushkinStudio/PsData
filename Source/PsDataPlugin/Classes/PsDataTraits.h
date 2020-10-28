// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PsDataStringView.h"

#include <type_traits>

/***********************************
 * TAlwaysFalse trait
 ***********************************/

namespace PsDataTools
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

#define CLANG defined(__clang__)

#if CLANG
#define __FUNCTION_SIGNATURE__ __PRETTY_FUNCTION__
#else
#define __FUNCTION_SIGNATURE__ __FUNCSIG__
#endif

constexpr TStringView GetSignature(const char* s)
{
	const TStringView Prefix("FType<");
	const TStringView Postfix(">::ContentType");
	TStringView Signature(s);

	Signature.RightChopInline(Signature.Find(Prefix) + Prefix.Len());
	Signature.LeftInline(Signature.Find(Postfix));

	return Signature;
}

template <typename T>
struct FType
{
	static FString ContentType()
	{
		constexpr TStringView Signature = GetSignature(__FUNCTION_SIGNATURE__);
		FString Result = FString(Signature.Len(), Signature.GetData());

		Result.ReplaceInline(TEXT("> "), TEXT(">"));

#if !CLANG
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
		return TStringView(__FUNCTION_SIGNATURE__).GetHash();
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
		return TStringView(__FUNCTION_SIGNATURE__).GetHash();
	}
};

template <typename T>
struct FType<TArray<T>>
{
	static FString ContentType()
	{
		return FType<TRemovePointer<T>::Type>::ContentType();
	}

	static FString Type()
	{
		return FString::Printf(TEXT("TArray<%s>"), *FType<T>::Type());
	}

	static constexpr uint32 Hash()
	{
		return TStringView(__FUNCTION_SIGNATURE__).GetHash();
	}
};

template <typename T>
struct FType<TMap<FString, T>>
{
	static FString ContentType()
	{
		return FType<TRemovePointer<T>::Type>::ContentType();
	}

	static FString Type()
	{
		return FString::Printf(TEXT("TMap<FString, %s>"), *FType<T>::Type());
	}

	static constexpr uint32 Hash()
	{
		return TStringView(__FUNCTION_SIGNATURE__).GetHash();
	}
};

} // namespace PsDataTools
