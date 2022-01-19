// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataUtils.h"

#include "CoreMinimal.h"

#include "PsDataFixedPoint.generated.h"

using PsDataFixedPointBaseType = int64;
#define FIXED_POINT_PRECISION 4

USTRUCT(BlueprintType, Blueprintable)
struct PSDATA_API FPsDataFixedPoint
{
	GENERATED_BODY()

private:
	PsDataFixedPointBaseType Base;

public:
	static const PsDataFixedPointBaseType Exp;

	static const FPsDataFixedPoint Zero;
	static const FPsDataFixedPoint One;
	static const FPsDataFixedPoint Two;
	static const FPsDataFixedPoint HalfUnit;
	static const FPsDataFixedPoint MinusOne;
	static const FPsDataFixedPoint Max;
	static const FPsDataFixedPoint Min;

	FPsDataFixedPoint();
	FPsDataFixedPoint(int32 Value);
	FPsDataFixedPoint(int64 Value);
	explicit FPsDataFixedPoint(float Value);
	explicit FPsDataFixedPoint(double Value);
	explicit FPsDataFixedPoint(const FString& Value);
	explicit FPsDataFixedPoint(const char* Value);

	void Set(const FPsDataFixedPoint& Other);

	FPsDataFixedPoint Floor() const;
	FPsDataFixedPoint Ceil() const;
	FPsDataFixedPoint Round() const;
	FPsDataFixedPoint Abs() const;

	bool operator<(const FPsDataFixedPoint& Other) const;
	bool operator<=(const FPsDataFixedPoint& Other) const;
	bool operator>(const FPsDataFixedPoint& Other) const;
	bool operator>=(const FPsDataFixedPoint& Other) const;
	bool operator==(const FPsDataFixedPoint& Other) const;
	bool operator!=(const FPsDataFixedPoint& Other) const;

	FPsDataFixedPoint operator+(const FPsDataFixedPoint& Value) const;
	FPsDataFixedPoint& operator+=(const FPsDataFixedPoint& Value);
	FPsDataFixedPoint operator-(const FPsDataFixedPoint& Value) const;
	FPsDataFixedPoint& operator-=(const FPsDataFixedPoint& Value);
	FPsDataFixedPoint operator*(const FPsDataFixedPoint& Value) const;
	FPsDataFixedPoint& operator*=(const FPsDataFixedPoint& Value);
	FPsDataFixedPoint operator/(const FPsDataFixedPoint& Value) const;
	FPsDataFixedPoint& operator/=(const FPsDataFixedPoint& Value);

	FPsDataFixedPoint& operator++();
	FPsDataFixedPoint& operator--();
	FPsDataFixedPoint operator-() const;
	FPsDataFixedPoint operator+() const;

	int64 ToInt() const;
	float ToFloat() const;
	FString ToString() const;
	PsDataFixedPointBaseType ToBase() const;

	static FPsDataFixedPoint FromString(const FString& Value);
	static FPsDataFixedPoint FromBase(PsDataFixedPointBaseType Value);

	bool ExportTextItem(FString& ValueStr, FPsDataFixedPoint const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const;
	bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText);
};

template <>
struct TStructOpsTypeTraits<FPsDataFixedPoint> : public TStructOpsTypeTraitsBase2<FPsDataFixedPoint>
{
	enum
	{
		WithExportTextItem = true,
		WithImportTextItem = true
	};
};

namespace PsDataTools
{
namespace Numbers
{
template <>
struct TSpecificNumber<FPsDataFixedPoint>
{
	template <typename T>
	static TOptional<FPsDataFixedPoint> DeserializeUnsignedInteger(const TDataStringView<T>& StringNumber)
	{
		if (auto UnsignedInteger = ToUnsignedInteger<PsDataFixedPointBaseType>(StringNumber))
		{
			return FPsDataFixedPoint(*UnsignedInteger);
		}

		return {};
	}

	template <typename T>
	static void Serialize(FPsDataFixedPoint Value, TArray<T>& Buffer)
	{
		constexpr int32 Length = GetDigitsNum<PsDataFixedPointBaseType>() + FIXED_POINT_PRECISION + 2;
		T CharBuffer[Length];
		int32 Pos = Length;

		auto Frac = Value.ToBase() % FPsDataFixedPoint::Exp;
		if (Frac != 0)
		{
			for (int32 i = 0; i < FIXED_POINT_PRECISION; ++i)
			{
				CharBuffer[--Pos] = DigitToChar<T>(Frac % 10);
				Frac /= 10;
			}

			CharBuffer[--Pos] = '.';
		}

		auto A = Value.ToBase() / FPsDataFixedPoint::Exp;
		do
		{
			CharBuffer[--Pos] = DigitToChar<T>(A % 10);
			A /= 10;
		}
		while (A != 0);

		if (Value < FPsDataFixedPoint::Zero)
		{
			CharBuffer[--Pos] = '-';
		}

		Buffer.Append(&CharBuffer[Pos], Length - Pos);
	}
};
} // namespace Numbers
} // namespace PsDataTools