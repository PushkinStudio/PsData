// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Types/PsData_FPsDataFixedPoint.h"

#define ZERO_DIVIDE_PROTECTION(Dividend, Divisor)                                                                                                                                   \
	if (Divisor == 0)                                                                                                                                                               \
	{                                                                                                                                                                               \
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Divide by zero detected: %s / 0\n%s"), *Dividend.ToString(), *FFrame::GetScriptCallstack()), ELogVerbosity::Warning); \
		return 0;                                                                                                                                                                   \
	}

//
// Literals
//

FPsDataFixedPoint UPsDataFixedPointLibrary::MakeLiteralFixedPoint(int32 Value)
{
	return FPsDataFixedPoint(Value);
}

FPsDataFixedPoint UPsDataFixedPointLibrary::MakeLiteralFixedPointFromInt64(int64 Value)
{
	return FPsDataFixedPoint(Value);
}

FPsDataFixedPoint UPsDataFixedPointLibrary::MakeLiteralFixedPointFromFloat(float Value)
{
	return FPsDataFixedPoint(Value);
}

FPsDataFixedPoint UPsDataFixedPointLibrary::MakeLiteralFixedPointFromString(FString Value)
{
	return FPsDataFixedPoint::FromString(Value);
}

//
// FixedPoint & FixedPoint
//

FPsDataFixedPoint UPsDataFixedPointLibrary::Multiply(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A * B;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Divide(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return A / B;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Add(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A + B;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Subtract(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A - B;
}

bool UPsDataFixedPointLibrary::Less(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A < B;
}

bool UPsDataFixedPointLibrary::Greater(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A > B;
}

bool UPsDataFixedPointLibrary::LessEqual(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A <= B;
}

bool UPsDataFixedPointLibrary::GreaterEqual(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A >= B;
}

bool UPsDataFixedPointLibrary::Equal(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A == B;
}

bool UPsDataFixedPointLibrary::NotEqual(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	return A != B;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Sign(FPsDataFixedPoint A)
{
	if (A > FPsDataFixedPoint::Zero)
	{
		return FPsDataFixedPoint::One;
	}
	else if (A < FPsDataFixedPoint::Zero)
	{
		return -FPsDataFixedPoint::One;
	}

	return FPsDataFixedPoint::Zero;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Min(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	if (A < B)
	{
		return A;
	}

	return B;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Max(FPsDataFixedPoint A, FPsDataFixedPoint B)
{
	if (A > B)
	{
		return A;
	}

	return B;
}

bool UPsDataFixedPointLibrary::InRange(FPsDataFixedPoint Value, FPsDataFixedPoint Min, FPsDataFixedPoint Max, bool InclusiveMin, bool InclusiveMax)
{
	if (Min < Value && Value < Max)
	{
		return true;
	}

	if (InclusiveMin && Min == Value)
	{
		return true;
	}

	if (InclusiveMax && Max == Value)
	{
		return true;
	}

	return false;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Clamp(FPsDataFixedPoint Value, FPsDataFixedPoint Min, FPsDataFixedPoint Max)
{
	if (Min > Value)
	{
		return Min;
	}

	if (Max < Value)
	{
		return Max;
	}

	return Value;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Abs(FPsDataFixedPoint A)
{
	return A < FPsDataFixedPoint::Zero ? -A : A;
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Floor(FPsDataFixedPoint Value)
{
	return Value.Floor();
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Ceil(FPsDataFixedPoint Value)
{
	return Value.Ceil();
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Round(FPsDataFixedPoint Value)
{
	return Value.Round();
}

//
// FixedPoint autocast
//

int64 UPsDataFixedPointLibrary::ToInt(const FPsDataFixedPoint& InFixedPoint)
{
	return InFixedPoint.ToInt();
}

float UPsDataFixedPointLibrary::ToFloat(const FPsDataFixedPoint& InFixedPoint)
{
	return InFixedPoint.ToFloat();
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Int32ToFixedPoint(const int32& InInt)
{
	return FPsDataFixedPoint(InInt);
}

FPsDataFixedPoint UPsDataFixedPointLibrary::Int64ToFixedPoint(const int64& InInt)
{
	return FPsDataFixedPoint(InInt);
}

FPsDataFixedPoint UPsDataFixedPointLibrary::FloatToFixedPoint(const float& InFloat)
{
	return FPsDataFixedPoint(InFloat);
}

FString UPsDataFixedPointLibrary::ToString(const FPsDataFixedPoint& InFixedPoint)
{
	return InFixedPoint.ToString();
}

FPsDataFixedPoint UPsDataFixedPointLibrary::StringToFixedPoint(const FString& InString)
{
	return FPsDataFixedPoint::FromString(InString);
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FPsDataFixedPoint, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FPsDataFixedPoint>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FPsDataFixedPoint, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FPsDataFixedPoint>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FPsDataFixedPoint, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FPsDataFixedPoint>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FPsDataFixedPoint, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FPsDataFixedPoint>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FPsDataFixedPoint, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FPsDataFixedPoint>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FPsDataFixedPoint, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FPsDataFixedPoint* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FPsDataFixedPoint, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FPsDataFixedPoint* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFixedPointLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FPsDataFixedPoint, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FPsDataFixedPoint>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFixedPointLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FPsDataFixedPoint& Value)
{
	Serializer->WriteValue(Value.ToString());
}

FPsDataFixedPoint UPsDataFixedPointLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FPsDataFixedPoint& Value)
{
	FString StringValue;
	if (Deserializer->ReadValue(StringValue))
	{
		return FPsDataFixedPoint(StringValue);
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FPsDataFixedPoint>::Type());
	return Value;
}