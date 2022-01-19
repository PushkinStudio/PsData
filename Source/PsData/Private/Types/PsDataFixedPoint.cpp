// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Types/PsDataFixedPoint.h"

// We use the number 10^n because it is serialization safe and human readable
const PsDataFixedPointBaseType FPsDataFixedPoint::Exp = PsDataTools::Numbers::Pow10<PsDataFixedPointBaseType>(FIXED_POINT_PRECISION);

const FPsDataFixedPoint FPsDataFixedPoint::Zero = 0;
const FPsDataFixedPoint FPsDataFixedPoint::One = 1;
const FPsDataFixedPoint FPsDataFixedPoint::Two = 2;
const FPsDataFixedPoint FPsDataFixedPoint::HalfUnit = One / Two;
const FPsDataFixedPoint FPsDataFixedPoint::MinusOne = -1;
const FPsDataFixedPoint FPsDataFixedPoint::Max = FPsDataFixedPoint::FromBase(std::numeric_limits<PsDataFixedPointBaseType>::max());
const FPsDataFixedPoint FPsDataFixedPoint::Min = FPsDataFixedPoint::FromBase(std::numeric_limits<PsDataFixedPointBaseType>::min());

FPsDataFixedPoint::FPsDataFixedPoint()
	: Base(0)
{
}

FPsDataFixedPoint::FPsDataFixedPoint(int32 Value)
	: Base(Value * Exp)
{
}

FPsDataFixedPoint::FPsDataFixedPoint(int64 Value)
	: Base(Value * Exp)
{
}

FPsDataFixedPoint::FPsDataFixedPoint(float Value)
	: Base(static_cast<PsDataFixedPointBaseType>(static_cast<double>(Value) * static_cast<double>(Exp)))
{
}

FPsDataFixedPoint::FPsDataFixedPoint(double Value)
	: Base(static_cast<PsDataFixedPointBaseType>(Value * static_cast<double>(Exp)))
{
}

FPsDataFixedPoint::FPsDataFixedPoint(const FString& Value)
{
	const auto View = PsDataTools::ToStringView(Value);
	if (auto Result = PsDataTools::Numbers::ToNumber<FPsDataFixedPoint>(View))
	{
		Set(*Result);
	}
	else
	{
		UE_LOG(LogDataUtils, Fatal, TEXT("Can't deserialize \"%s\" to FPsDataFixedPoint"), *Value);
	}
}

FPsDataFixedPoint::FPsDataFixedPoint(const char* Value)
{
	const auto View = PsDataTools::ToStringView(Value);
	if (auto Result = PsDataTools::Numbers::ToNumber<FPsDataFixedPoint>(View))
	{
		Set(*Result);
	}
	else
	{
		UE_LOG(LogDataUtils, Fatal, TEXT("Can't deserialize \"%s\" to FPsDataFixedPoint"), *PsDataTools::ToString(View));
	}
}

void FPsDataFixedPoint::Set(const FPsDataFixedPoint& Other)
{
	Base = Other.Base;
}

FPsDataFixedPoint FPsDataFixedPoint::Floor() const
{
	if (Base % Exp == 0)
	{
		return FPsDataFixedPoint(Base / Exp);
	}

	if (Base >= 0)
	{
		return FPsDataFixedPoint(Base / Exp);
	}
	return FPsDataFixedPoint(Base / Exp - 1);
}

FPsDataFixedPoint FPsDataFixedPoint::Ceil() const
{
	if (Base % Exp == 0)
	{
		return FPsDataFixedPoint(Base / Exp);
	}

	if (Base >= 0)
	{
		return FPsDataFixedPoint(Base / Exp + 1);
	}
	return FPsDataFixedPoint(Base / Exp);
}

FPsDataFixedPoint FPsDataFixedPoint::Round() const
{
	if (Base % Exp == 0)
	{
		return FPsDataFixedPoint(Base / Exp);
	}

	if (Base >= 0)
	{
		return FPsDataFixedPoint((Base + HalfUnit.Base) / Exp);
	}
	return FPsDataFixedPoint((Base - HalfUnit.Base) / Exp);
}

FPsDataFixedPoint FPsDataFixedPoint::Abs() const
{
	if (Base < 0)
	{
		return FromBase(-Base);
	}
	return FromBase(Base);
}

bool FPsDataFixedPoint::operator<(const FPsDataFixedPoint& Other) const
{
	return Base < Other.Base;
}

bool FPsDataFixedPoint::operator<=(const FPsDataFixedPoint& Other) const
{
	return Base <= Other.Base;
}

bool FPsDataFixedPoint::operator>(const FPsDataFixedPoint& Other) const
{
	return Base > Other.Base;
}

bool FPsDataFixedPoint::operator>=(const FPsDataFixedPoint& Other) const
{
	return Base >= Other.Base;
}

bool FPsDataFixedPoint::operator==(const FPsDataFixedPoint& Other) const
{
	return Base == Other.Base;
}

bool FPsDataFixedPoint::operator!=(const FPsDataFixedPoint& Other) const
{
	return Base != Other.Base;
}

FPsDataFixedPoint FPsDataFixedPoint::operator+(const FPsDataFixedPoint& Value) const
{
	return FromBase(Base + Value.Base);
}

FPsDataFixedPoint& FPsDataFixedPoint::operator+=(const FPsDataFixedPoint& Value)
{
	Set(FromBase(Base) + Value);
	return *this;
}

FPsDataFixedPoint FPsDataFixedPoint::operator-(const FPsDataFixedPoint& Value) const
{
	return FromBase(Base - Value.Base);
}

FPsDataFixedPoint& FPsDataFixedPoint::operator-=(const FPsDataFixedPoint& Value)
{
	Set(FromBase(Base) - Value);
	return *this;
}

FPsDataFixedPoint FPsDataFixedPoint::operator*(const FPsDataFixedPoint& Value) const
{
	//return FromBase((Base * Value.Base) / Exp);
	const PsDataFixedPointBaseType Ai = Base / Exp;
	const PsDataFixedPointBaseType Af = Base % Exp;
	const PsDataFixedPointBaseType Bi = Value.Base / Exp;
	const PsDataFixedPointBaseType Bf = Value.Base % Exp;
	return FromBase((Ai * Bi) * Exp + (Af * Bf) / Exp + Ai * Bf + Bi * Af);
}

FPsDataFixedPoint& FPsDataFixedPoint::operator*=(const FPsDataFixedPoint& Value)
{
	Set(FromBase(Base) * Value);
	return *this;
}

FPsDataFixedPoint FPsDataFixedPoint::operator/(const FPsDataFixedPoint& Value) const
{
	//TODO: Overflow!!!
	return FromBase((Base * Exp) / Value.Base);
}

FPsDataFixedPoint& FPsDataFixedPoint::operator/=(const FPsDataFixedPoint& Value)
{
	Set(FromBase(Base) / Value);
	return *this;
}

FPsDataFixedPoint& FPsDataFixedPoint::operator++()
{
	Set(FromBase(Base + One.Base));
	return *this;
}

FPsDataFixedPoint& FPsDataFixedPoint::operator--()
{
	Set(FromBase(Base - One.Base));
	return *this;
}

FPsDataFixedPoint FPsDataFixedPoint::operator-() const
{
	return FromBase(-Base);
}

FPsDataFixedPoint FPsDataFixedPoint::operator+() const
{
	return FromBase(Base);
}

int64 FPsDataFixedPoint::ToInt() const
{
	return Base / Exp;
}

float FPsDataFixedPoint::ToFloat() const
{
	return static_cast<float>(static_cast<double>(Base) / static_cast<double>(Exp));
}

FString FPsDataFixedPoint::ToString() const
{
	return PsDataTools::Numbers::ToString(*this);
}

PsDataFixedPointBaseType FPsDataFixedPoint::ToBase() const
{
	return Base;
}

FPsDataFixedPoint FPsDataFixedPoint::FromString(const FString& Value)
{
	if (auto Result = PsDataTools::Numbers::ToNumber<FPsDataFixedPoint>(PsDataTools::ToStringView(Value)))
	{
		return *Result;
	}

	UE_LOG(LogDataUtils, Warning, TEXT("Can't deserialize \"%s\" to FPsDataFixedPoint"), *Value);

	return Zero;
}

FPsDataFixedPoint FPsDataFixedPoint::FromBase(PsDataFixedPointBaseType Value)
{
	FPsDataFixedPoint Result;
	Result.Base = Value;
	return Result;
}

bool FPsDataFixedPoint::ExportTextItem(FString& ValueStr, FPsDataFixedPoint const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
	ValueStr = ToString();
	return true;
}

bool FPsDataFixedPoint::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	auto Result = PsDataTools::Numbers::ToNumber<FPsDataFixedPoint>(PsDataTools::ToStringView(Buffer));
	if (Result)
	{
		*this = *Result;
		return true;
	}

	return false;
}