// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataStringView.h"

#include "CoreMinimal.h"

#include "PsDataTraits.h"
#include "math.h"

#include <cmath>
#include <limits>

DEFINE_LOG_CATEGORY_STATIC(LogDataUtils, VeryVerbose, All);

namespace PsDataTools
{

template <typename T>
constexpr bool IsValidCharForKey(T Char)
{
	return (Char >= 'a' && Char <= 'z') || (Char >= 'A' && Char <= 'Z') || (Char >= '0' && Char <= '9') || Char == '_' || Char == '-';
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

namespace Numbers
{
template <typename K>
struct TSpecificNumber
{
};

template <typename T>
constexpr bool IsDot(T Char)
{
	return Char == '.' || Char == ',';
}

template <typename T>
constexpr bool IsExp(T Char)
{
	return Char == 'e' || Char == 'E';
}

template <typename T>
constexpr bool IsUndefinedFloatingPoint(const TDataStringView<T>& StringNumber)
{
	const auto Len = StringNumber.Len();
	if (Len == 3)
	{
		return Utils::Equal<true>(StringNumber.GetData(), Len, "nan", Len) ||
			   Utils::Equal<true>(StringNumber.GetData(), Len, "inf", Len);
	}
	if (Len == 4)
	{
		return Utils::Equal<true>(StringNumber.GetData(), Len, "null", Len) ||
			   Utils::Equal<true>(StringNumber.GetData(), Len, "+inf", Len) ||
			   Utils::Equal<true>(StringNumber.GetData(), Len, "-inf", Len);
	}

	return false;
}

enum
{
	CHAR_TO_DIGIT_FAIL = -1,
	DIGIT_TO_CHAR_FAIL = 0
};

template <typename T>
constexpr int32 CharToDigit(T Char)
{
	if (Char >= '0' && Char <= '9')
	{
		return Char - '0';
	}

	return CHAR_TO_DIGIT_FAIL;
}

template <typename T>
constexpr T DigitToChar(int32 Number)
{
	if (Number < 0)
	{
		Number = -Number;
	}

	if (Number <= 9)
	{
		return '0' + Number;
	}

	return DIGIT_TO_CHAR_FAIL;
}

template <typename T>
constexpr bool IsUnsignedInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		for (int32 i = 0; i < StringNumber.Len(); ++i)
		{
			if (CharToDigit(StringNumber[i]) == CHAR_TO_DIGIT_FAIL)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

template <typename T>
constexpr bool IsInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		const T Char = StringNumber[0];
		if (Char == '-' || Char == '+')
		{
			return IsUnsignedInteger(StringNumber.RightChop(1));
		}
		return IsUnsignedInteger(StringNumber);
	}
	return false;
}

template <typename T>
constexpr bool IsNumber(const TDataStringView<T>& StringNumber)
{
	const auto DotPos = StringNumber.FindByPredicate([](T Char) { return IsDot(Char); });
	const auto ExpPos = StringNumber.FindByPredicate([](T Char) { return IsExp(Char); });

	if (DotPos == INDEX_NONE)
	{
		if (ExpPos == INDEX_NONE)
		{
			if (IsUndefinedFloatingPoint(StringNumber))
			{
				return true;
			}

			return IsInteger(StringNumber);
		}
		else
		{
			return IsInteger(StringNumber.Left(ExpPos)) &&
				   IsInteger(StringNumber.RightChop(ExpPos + 1));
		}
	}
	else
	{
		if (ExpPos == INDEX_NONE)
		{
			return IsInteger(StringNumber.Left(DotPos)) &&
				   IsUnsignedInteger(StringNumber.RightChop(DotPos + 1));
		}
		else
		{
			return IsInteger(StringNumber.Left(DotPos)) &&
				   IsUnsignedInteger(StringNumber.RightChop(DotPos + 1).Left(ExpPos - DotPos - 1)) &&
				   IsInteger(StringNumber.RightChop(ExpPos + 1));
		}
	}
}

template <typename K>
constexpr K Pow10x(K X, int32 Pow)
{
	if (Pow > 0)
	{
		K Base = 10;
		K PrevBase = Base;
		while (Pow > 0)
		{
			if (Pow & 0x1)
			{
				X *= Base;
			}
			PrevBase = Base;
			Base *= Base;
			if (PrevBase > Base)
			{
				return 0;
			}
			Pow >>= 1;
		}
	}
	else if (Pow < 0)
	{
		Pow = -Pow;
		K Base = 10;
		K PrevBase = Base;
		while (Pow > 0)
		{
			if (Pow & 0x1)
			{
				X /= Base;
			}
			PrevBase = Base;
			Base *= Base;
			if (PrevBase > Base)
			{
				return 0;
			}
			Pow >>= 1;
		}
	}

	return X;
}

template <typename K>
constexpr K Pow10(int32 Pow)
{
	return Pow10x<K>(1, Pow);
}

template <int32 Bits>
constexpr int32 GetDigitsNumByBitsNum()
{
	// bits / log2(10)
	return static_cast<int32>(static_cast<float>(Bits) / 3.3219280949f);
}

template <typename K>
constexpr int32 GetDigitsNum()
{
	static_assert(std::is_integral<K>::value, "Only for integer types");
	return GetDigitsNumByBitsNum<std::numeric_limits<K>::digits>();
}

template <typename K, typename T>
typename TEnableIf<std::numeric_limits<K>::is_specialized, TOptional<K>>::Type
ToUnsignedInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		K Result = 0;
		for (int32 i = 0; i < StringNumber.Len(); ++i)
		{
			const auto Digit = CharToDigit(StringNumber[i]);
			if (Digit != CHAR_TO_DIGIT_FAIL)
			{
				Result = Result * 10 + Digit;
			}
			else
			{
				return {};
			}
		}
		return Result;
	}
	return {};
}

template <typename K, typename T>
typename TEnableIf<!std::numeric_limits<K>::is_specialized, TOptional<K>>::Type
ToUnsignedInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		return TSpecificNumber<K>::DeserializeUnsignedInteger(StringNumber);
	}
	return {};
}

template <typename K, typename T>
typename TEnableIf<std::numeric_limits<K>::is_specialized && std::numeric_limits<K>::is_signed, TOptional<K>>::Type
ToInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		const T Char = StringNumber[0];
		if (Char == '-')
		{
			if (const auto UnsignedInteger = ToUnsignedInteger<K>(StringNumber.RightChop(1)))
			{
				return -UnsignedInteger.GetValue();
			}

			return {};
		}
		if (Char == '+')
		{
			return ToUnsignedInteger<K>(StringNumber.RightChop(1));
		}
		return ToUnsignedInteger<K>(StringNumber);
	}
	return {};
}

template <typename K, typename T>
typename TEnableIf<std::numeric_limits<K>::is_specialized && !std::numeric_limits<K>::is_signed, TOptional<K>>::Type
ToInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		const T Char = StringNumber[0];
		if (Char == '-')
		{
			return {};
		}
		if (Char == '+')
		{
			return ToUnsignedInteger<K>(StringNumber.RightChop(1));
		}
		return ToUnsignedInteger<K>(StringNumber);
	}
	return {};
}

template <typename K, typename T>
typename TEnableIf<!std::numeric_limits<K>::is_specialized, TOptional<K>>::Type
ToInteger(const TDataStringView<T>& StringNumber)
{
	if (StringNumber.Len() > 0)
	{
		const T Char = StringNumber[0];
		if (Char == '-')
		{
			if (const auto UnsignedInteger = ToUnsignedInteger<K>(StringNumber.RightChop(1)))
			{
				return -UnsignedInteger.GetValue();
			}

			return {};
		}
		if (Char == '+')
		{
			return ToUnsignedInteger<K>(StringNumber.RightChop(1));
		}
		return ToUnsignedInteger<K>(StringNumber);
	}
	return {};
}

template <typename K>
typename TEnableIf<std::is_floating_point<K>::value, K>::Type
GetUndefinedFloatingPoint()
{
	return NAN;
}

template <typename K>
typename TEnableIf<!std::is_floating_point<K>::value, K>::Type
GetUndefinedFloatingPoint()
{
	ensureMsgf(false, TEXT("NaN is not supported for this type"));
	return 0;
}

template <typename K, typename T>
TOptional<K> ToNumber(const TDataStringView<T>& StringNumber)
{
	const auto DotPos = StringNumber.FindByPredicate([](T Char) { return IsDot(Char); });
	const auto ExpPos = StringNumber.FindByPredicate([](T Char) { return IsExp(Char); });

	if (DotPos == INDEX_NONE)
	{
		if (ExpPos == INDEX_NONE)
		{
			if (IsUndefinedFloatingPoint(StringNumber))
			{
				return GetUndefinedFloatingPoint<K>();
			}

			return ToInteger<K>(StringNumber);
		}
		else
		{
			const auto AStr = StringNumber.Left(ExpPos);
			if (const auto A = ToInteger<K>(AStr))
			{
				const auto CStr = StringNumber.RightChop(ExpPos + 1);
				if (const auto C = ToInteger<int32>(CStr))
				{
					return Pow10x<K>(A.GetValue(), C.GetValue());
				}
			}
		}
	}
	else
	{
		if (ExpPos == INDEX_NONE)
		{
			const auto AStr = StringNumber.Left(DotPos);
			if (const auto A = ToInteger<K>(AStr))
			{
				const auto BStr = StringNumber.RightChop(DotPos + 1);
				if (const auto B = ToUnsignedInteger<K>(BStr))
				{
					if (A.GetValue() < 0)
					{
						return A.GetValue() - Pow10x<K>(B.GetValue(), -BStr.Len());
					}
					else
					{
						return A.GetValue() + Pow10x<K>(B.GetValue(), -BStr.Len());
					}
				}
			}
		}
		else
		{
			const auto AStr = StringNumber.Left(DotPos);
			if (const auto A = ToInteger<K>(AStr))
			{
				const auto BStr = StringNumber.RightChop(DotPos + 1).Left(ExpPos - DotPos - 1);
				if (const auto B = ToUnsignedInteger<K>(BStr))
				{
					const auto CStr = StringNumber.RightChop(ExpPos + 1);
					if (const auto C = ToInteger<int32>(CStr))
					{
						if (A.GetValue() < 0)
						{
							return Pow10x<K>(A.GetValue(), C.GetValue()) - Pow10x<K>(B.GetValue(), C.GetValue() - BStr.Len());
						}
						else
						{
							return Pow10x<K>(A.GetValue(), C.GetValue()) + Pow10x<K>(B.GetValue(), C.GetValue() - BStr.Len());
						}
					}
				}
			}
		}
	}

	return {};
}

template <typename K, typename T>
typename TEnableIf<std::is_integral<K>::value, void>::Type
ToString(K Value, TArray<T>& Buffer)
{
	constexpr bool bCanBeNegative = std::numeric_limits<K>::is_signed;
	constexpr int32 Length = GetDigitsNum<K>() + 1 + (bCanBeNegative ? 1 : 0);
	const bool bNegative = bCanBeNegative && Value < 0;

	T CharBuffer[Length];
	int32 Pos = Length;
	do
	{
		CharBuffer[--Pos] = DigitToChar<T>(Value % 10);
		Value /= 10;
	}
	while (Value != 0);

	if (bNegative)
	{
		CharBuffer[--Pos] = '-';
	}

	Buffer.Append(&CharBuffer[Pos], Length - Pos);
}

template <typename K, typename T>
typename TEnableIf<std::is_floating_point<K>::value, void>::Type
ToString(K Value, TArray<T>& Buffer)
{
	constexpr int32 Precision = 6;

	auto FloatType = std::fpclassify(Value);
	if (FloatType == FP_NAN || FloatType == FP_INFINITE)
	{
		Buffer.Append(TEXT("null"), 4); // ECMA-404.8
	}
	else if (FloatType == FP_ZERO)
	{
		Buffer.Add('0');
	}
	else
	{
		using FUnsignedType = typename TIntegerEquivalent<K>::UnsignedType;
		constexpr K MaxValue = Pow10<K>(GetDigitsNum<FUnsignedType>());
		constexpr FUnsignedType InvEpsilon = Pow10<FUnsignedType>(Precision);
		constexpr K Epsilon = static_cast<K>(1) / static_cast<K>(InvEpsilon);

		/**
		 * Default buffer size: "±" + A(minimum 10 digits) + "." + B(Precision) ≈ 18
		 * Scientific notation buffer size: "±" + A(always 1 digit) + "." + B(Precision) + "e±" + E(maximum 4 digits) ≈ 15
		 */
		constexpr int32 Length = 1 + (GetDigitsNum<FUnsignedType>() + 1) + 1 + Precision;

		T CharBuffer[Length];
		int32 Pos = Length;

		const bool bNegative = Value < 0;
		if (bNegative)
		{
			Value = -Value;
		}

		const bool bScientificNotation = (Value < Epsilon || Value > MaxValue);
		if (bScientificNotation)
		{
			auto Exp = static_cast<int32>(std::floor(std::log10(Value)));
			Value = Pow10x<K>(Value, -Exp);

			const bool bExpNegative = Exp < 0;
			do
			{
				CharBuffer[--Pos] = DigitToChar<T>(Exp % 10);
				Exp /= 10;
			}
			while (Exp != 0);

			if (bExpNegative)
			{
				CharBuffer[--Pos] = '-';
			}

			CharBuffer[--Pos] = 'e';
		}

		FUnsignedType A = static_cast<FUnsignedType>(Value);
		FUnsignedType B = static_cast<FUnsignedType>((Value - A) * InvEpsilon + 0.5);

		if (B != 0)
		{
			int32 Num = Precision;
			while (Num > 0)
			{
				CharBuffer[--Pos] = DigitToChar<T>(B % 10);
				B /= 10;
				--Num;
			}

			CharBuffer[--Pos] = '.';
		}

		do
		{
			CharBuffer[--Pos] = DigitToChar<T>(A % 10);
			A /= 10;
		}
		while (A != 0);

		if (bNegative)
		{
			CharBuffer[--Pos] = '-';
		}

		Buffer.Append(&CharBuffer[Pos], Length - Pos);
	}
}

template <typename K, typename T>
typename TEnableIf<!std::numeric_limits<K>::is_specialized, void>::Type
ToString(K Value, TArray<T>& Buffer)
{
	TSpecificNumber<K>::Serialize(Value, Buffer);
}

template <typename K>
FString ToString(K Value)
{
	TArray<TCHAR> Buffer;
	ToString(Value, Buffer);
	return FString(Buffer.Num(), Buffer.GetData());
}

} // namespace Numbers
} // namespace PsDataTools