// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataStringView.h"

#include "CoreMinimal.h"

namespace PsDataTools
{

template <typename T>
constexpr bool IsValidCharForKey(T c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-';
}

template <typename T>
constexpr bool IsValidKey(const T* Data)
{
	if (*Data)
	{
		while (*Data++)
		{
			if (!IsValidCharForKey(*Data))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

template <typename T>
constexpr bool IsValidKey(const T* Data, int32 Length)
{
	if (Length > 0)
	{
		for (int32 i = 0; i < Length; ++i)
		{
			if (!IsValidCharForKey(Data[i]))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

template <typename T>
constexpr bool IsValidKey(const TDataStringView<T>& Key)
{
	return IsValidKey(Key.GetData(), Key.Len());
}

FORCEINLINE bool IsValidKey(const FString& Key)
{
	return IsValidKey(Key.GetCharArray().GetData(), Key.Len());
}

struct PSDATA_API FDataNoncopyable
{
protected:
	FDataNoncopyable() {}
	~FDataNoncopyable() {}

private:
	FDataNoncopyable(const FDataNoncopyable&) = delete;
	FDataNoncopyable& operator=(const FDataNoncopyable&) = delete;
};

struct PSDATA_API FDataNonmovable
{
protected:
	FDataNonmovable() {}
	~FDataNonmovable() {}

private:
	FDataNonmovable(FDataNonmovable&&) = delete;
	FDataNonmovable& operator=(FDataNonmovable&&) = delete;
};

} // namespace PsDataTools