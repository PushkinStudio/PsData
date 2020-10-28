// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_FPsDataBigInteger.h"

#define ZERO_DIVIDE_PROTECTION(Dividend, Divisor)                                                                                                                                   \
	if (Divisor == 0)                                                                                                                                                               \
	{                                                                                                                                                                               \
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Divide by zero detected: %s / 0\n%s"), *Dividend.ToString(), *FFrame::GetScriptCallstack()), ELogVerbosity::Warning); \
		return 0;                                                                                                                                                                   \
	}

//
// Literals
//

FPsDataBigInteger UPsDataBigIntegerLibrary::MakeLiteralBigInteger(int64 Value)
{
	return {Value};
}

FPsDataBigInteger UPsDataBigIntegerLibrary::MakeLiteralBigIntegerFromInt(int32 Value)
{
	return {Value};
}

FPsDataBigInteger UPsDataBigIntegerLibrary::MakeLiteralBigIntegerFromString(FString Value, EPsDataBigIntegerConvertionType ConvertionType)
{
	return FPsDataBigInteger::FromString(Value, ConvertionType);
}

//
// BigInteger & BigInteger
//

FPsDataBigInteger UPsDataBigIntegerLibrary::Multiply(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A * B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Divide(FPsDataBigInteger A, FPsDataBigInteger B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return A / B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Add(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A + B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Subtract(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A - B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Modulo(FPsDataBigInteger A, FPsDataBigInteger B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return A % B;
}

bool UPsDataBigIntegerLibrary::Less(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A < B;
}

bool UPsDataBigIntegerLibrary::Greater(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A > B;
}

bool UPsDataBigIntegerLibrary::LessEqual(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A <= B;
}

bool UPsDataBigIntegerLibrary::GreaterEqual(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A >= B;
}

bool UPsDataBigIntegerLibrary::Equal(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A == B;
}

bool UPsDataBigIntegerLibrary::NotEqual(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A != B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::And(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A & B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Xor(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A ^ B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Or(FPsDataBigInteger A, FPsDataBigInteger B)
{
	return A | B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Not(FPsDataBigInteger A)
{
	return ~A;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Sign(FPsDataBigInteger A)
{
	if (A.IsZero())
	{
		return FPsDataBigInteger::Zero;
	}

	return A.GetSign();
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Min(FPsDataBigInteger A, FPsDataBigInteger B)
{
	if (A < B)
	{
		return A;
	}

	return B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Max(FPsDataBigInteger A, FPsDataBigInteger B)
{
	if (A > B)
	{
		return A;
	}

	return B;
}

bool UPsDataBigIntegerLibrary::InRange(FPsDataBigInteger Value, FPsDataBigInteger Min, FPsDataBigInteger Max, bool InclusiveMin, bool InclusiveMax)
{
	const auto CompMin = FPsDataBigInteger::Compare(Min, Value);
	const auto CompMax = FPsDataBigInteger::Compare(Value, Max);

	if (CompMin < 0 && CompMax > 0)
	{
		return true;
	}

	if (InclusiveMin && CompMin == 0)
	{
		return true;
	}

	if (InclusiveMax && CompMax == 0)
	{
		return true;
	}

	return false;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Clamp(FPsDataBigInteger Value, FPsDataBigInteger Min, FPsDataBigInteger Max)
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

FPsDataBigInteger UPsDataBigIntegerLibrary::Abs(FPsDataBigInteger A)
{
	A.Abs();
	return A;
}

bool UPsDataBigIntegerLibrary::Bit(FPsDataBigInteger A, int32 BitIndex)
{
	return A.GetBit(BitIndex);
}

//
// BigInteger autocast
//

FPsDataShortBigInteger UPsDataBigIntegerLibrary::ToShortBigInteger(const FPsDataBigInteger& InBigInteger)
{
	return InBigInteger.ToShortBigInteger(18);
}

int32 UPsDataBigIntegerLibrary::ToInt(const FPsDataBigInteger& InBigInteger)
{
	return InBigInteger.ToInt32();
}

int32 UPsDataBigIntegerLibrary::ToInt64(const FPsDataBigInteger& InBigInteger)
{
	return InBigInteger.ToInt64();
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Int32ToBigInteger(const int32& InInt)
{
	return {InInt};
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Int64ToBigInteger(const int64& InInt)
{
	return {InInt};
}

FString UPsDataBigIntegerLibrary::ToString(const FPsDataBigInteger& InBigInteger)
{
	return InBigInteger.ToString();
}

FPsDataBigInteger UPsDataBigIntegerLibrary::StringToBigInteger(const FString& InString)
{
	return FPsDataBigInteger::FromString(InString);
}

//
// BigInteger & int32
//

FPsDataBigInteger UPsDataBigIntegerLibrary::Multiply_Int32(FPsDataBigInteger A, int32 B)
{
	return A * B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Divide_Int32(FPsDataBigInteger A, int32 B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return A / B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Add_Int32(FPsDataBigInteger A, int32 B)
{
	return A + B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Subtract_Int32(FPsDataBigInteger A, int32 B)
{
	return A - B;
}

int32 UPsDataBigIntegerLibrary::Modulo_Int32(FPsDataBigInteger A, int32 B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return (A % B).ToInt32();
}

bool UPsDataBigIntegerLibrary::Less_Int32(FPsDataBigInteger A, int32 B)
{
	return A < B;
}

bool UPsDataBigIntegerLibrary::Greater_Int32(FPsDataBigInteger A, int32 B)
{
	return A > B;
}

bool UPsDataBigIntegerLibrary::LessEqual_Int32(FPsDataBigInteger A, int32 B)
{
	return A <= B;
}

bool UPsDataBigIntegerLibrary::GreaterEqual_Int32(FPsDataBigInteger A, int32 B)
{
	return A >= B;
}

bool UPsDataBigIntegerLibrary::Equal_Int32(FPsDataBigInteger A, int32 B)
{
	return A == B;
}

bool UPsDataBigIntegerLibrary::NotEqual_Int32(FPsDataBigInteger A, int32 B)
{
	return A != B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::And_Int32(FPsDataBigInteger A, int32 B)
{
	return A & B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Xor_Int32(FPsDataBigInteger A, int32 B)
{
	return A ^ B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Or_Int32(FPsDataBigInteger A, int32 B)
{
	return A | B;
}

//
// BigInteger & int64
//

FPsDataBigInteger UPsDataBigIntegerLibrary::Multiply_Int64(FPsDataBigInteger A, int64 B)
{
	return A * B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Divide_Int64(FPsDataBigInteger A, int64 B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return A / B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Add_Int64(FPsDataBigInteger A, int64 B)
{
	return A + B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Subtract_Int64(FPsDataBigInteger A, int64 B)
{
	return A - B;
}

int64 UPsDataBigIntegerLibrary::Modulo_Int64(FPsDataBigInteger A, int64 B)
{
	ZERO_DIVIDE_PROTECTION(A, B);
	return (A % B).ToInt64();
}

bool UPsDataBigIntegerLibrary::Less_Int64(FPsDataBigInteger A, int64 B)
{
	return A < B;
}

bool UPsDataBigIntegerLibrary::Greater_Int64(FPsDataBigInteger A, int64 B)
{
	return A > B;
}

bool UPsDataBigIntegerLibrary::LessEqual_Int64(FPsDataBigInteger A, int64 B)
{
	return A <= B;
}

bool UPsDataBigIntegerLibrary::GreaterEqual_Int64(FPsDataBigInteger A, int64 B)
{
	return A >= B;
}

bool UPsDataBigIntegerLibrary::Equal_Int64(FPsDataBigInteger A, int64 B)
{
	return A == B;
}

bool UPsDataBigIntegerLibrary::NotEqual_Int64(FPsDataBigInteger A, int64 B)
{
	return A != B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::And_Int64(FPsDataBigInteger A, int64 B)
{
	return A & B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Xor_Int64(FPsDataBigInteger A, int64 B)
{
	return A ^ B;
}

FPsDataBigInteger UPsDataBigIntegerLibrary::Or_Int64(FPsDataBigInteger A, int64 B)
{
	return A | B;
}

DEFINE_FUNCTION(UPsDataBigIntegerLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, FPsDataBigInteger, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TMap<FString, FPsDataBigInteger>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBigIntegerLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, FPsDataBigInteger, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FPsDataBigInteger>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBigIntegerLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(FPsDataBigInteger, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TArray<FPsDataBigInteger>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBigIntegerLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(FPsDataBigInteger, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FPsDataBigInteger>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBigIntegerLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_STRUCT_REF(FPsDataBigInteger, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<FPsDataBigInteger>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBigIntegerLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_STRUCT_REF(FPsDataBigInteger, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FPsDataBigInteger* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataBigIntegerLibrary::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FPsDataBigInteger& Value)
{
	Serializer->WriteValue(Value.ToString());
}

FPsDataBigInteger UPsDataBigIntegerLibrary::TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FPsDataBigInteger& Value)
{
	FString StringValue;
	if (Deserializer->ReadValue(StringValue))
	{
		auto a = FPsDataBigInteger(StringValue);
		return a;
	}

	int64 Int64Value = 0;
	if (Deserializer->ReadValue(Int64Value))
	{
		return Int64Value;
	}

	int32 Int32Value = 0;
	if (Deserializer->ReadValue(Int32Value))
	{
		return Int32Value;
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FPsDataBigInteger>::Type());
	return Value;
}