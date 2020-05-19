// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataAPI.h"
#include "Types/PsDataBigInteger.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsDataBigIntegerLibrary.generated.h"

UCLASS()
class PSDATAPLUGIN_API UPsDataBigIntegerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//
	// Literals
	//

	/** Make BigInteger from integer64 */
	UFUNCTION(BlueprintPure, Category = "Math|BigInteger")
	static FPsDataBigInteger MakeLiteralBigInteger(int64 Value);

	/** Make BigInteger from integer */
	UFUNCTION(BlueprintPure, Category = "Math|BigInteger")
	static FPsDataBigInteger MakeLiteralBigIntegerFromInt(int32 Value);

	/** Make BigInteger from string */
	UFUNCTION(BlueprintPure, Category = "Math|BigInteger")
	static FPsDataBigInteger MakeLiteralBigIntegerFromString(FString Value, EPsDataBigIntegerConvertionType ConvertionType = EPsDataBigIntegerConvertionType::Dec);

	//
	// BigInteger & BigInteger
	//

	/** Multiplication (A * B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger * BigInteger", CompactNodeTitle = "*", Keywords = "* multiply", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|BigInteger")
	static FPsDataBigInteger Multiply(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Division (A / B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger / BigInteger", CompactNodeTitle = "/", Keywords = "/ divide division"), Category = "Math|BigInteger")
	static FPsDataBigInteger Divide(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Addition (A + B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger + BigInteger", CompactNodeTitle = "+", Keywords = "+ add plus", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|BigInteger")
	static FPsDataBigInteger Add(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Subtraction (A - B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger - BigInteger", CompactNodeTitle = "-", Keywords = "- subtract minus"), Category = "Math|BigInteger")
	static FPsDataBigInteger Subtract(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Modulo (A % B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "% (BigInteger)", CompactNodeTitle = "%", Keywords = "% modulus"), Category = "Math|Integer")
	static FPsDataBigInteger Modulo(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if A is less than B (A < B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger < BigInteger", CompactNodeTitle = "<", Keywords = "< less"), Category = "Math|BigInteger")
	static bool Less(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if A is greater than B (A > B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger > BigInteger", CompactNodeTitle = ">", Keywords = "> greater"), Category = "Math|BigInteger")
	static bool Greater(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if A is less than or equal to B (A <= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger <= BigInteger", CompactNodeTitle = "<=", Keywords = "<= less"), Category = "Math|BigInteger")
	static bool LessEqual(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if A is greater than or equal to B (A >= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger >= BigInteger", CompactNodeTitle = ">=", Keywords = ">= greater"), Category = "Math|BigInteger")
	static bool GreaterEqual(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if A is equal to B (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (BigInteger)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math|BigInteger")
	static bool Equal(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if A is not equal to B (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (BigInteger)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "Math|BigInteger")
	static bool NotEqual(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Bitwise AND (A & B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise AND", CompactNodeTitle = "&", Keywords = "& and", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|BigInteger")
	static FPsDataBigInteger And(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Bitwise XOR (A ^ B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise XOR", CompactNodeTitle = "^", Keywords = "^ xor"), Category = "Math|BigInteger")
	static FPsDataBigInteger Xor(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Bitwise OR (A | B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise OR", CompactNodeTitle = "|", Keywords = "| or", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|BigInteger")
	static FPsDataBigInteger Or(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Bitwise NOT (~A) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise NOT", CompactNodeTitle = "~", Keywords = "~ not"), Category = "Math|BigInteger")
	static FPsDataBigInteger Not(FPsDataBigInteger A);

	/** Sign (returns -1 if A < 0, 0 if A is zero, and +1 if A > 0) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sign (BigInteger)"), Category = "Math|BigInteger")
	static FPsDataBigInteger Sign(FPsDataBigInteger A);

	/** Returns the minimum value of A and B */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Min (BigInteger)", CompactNodeTitle = "MIN", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|BigInteger")
	static FPsDataBigInteger Min(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns the maximum value of A and B */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Max (BigInteger)", CompactNodeTitle = "MAX", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|BigInteger")
	static FPsDataBigInteger Max(FPsDataBigInteger A, FPsDataBigInteger B);

	/** Returns true if value is between Min and Max (V >= Min && V <= Max)
	 * If InclusiveMin is true, value needs to be equal or larger than Min, else it needs to be larger
	 * If InclusiveMax is true, value needs to be smaller or equal than Max, else it needs to be smaller
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "InRange (BigInteger)", Min = "0", Max = "10"), Category = "Math|BigInteger")
	static bool InRange(FPsDataBigInteger Value, FPsDataBigInteger Min, FPsDataBigInteger Max, bool InclusiveMin = true, bool InclusiveMax = true);

	/** Returns Value clamped to be between A and B (inclusive) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Clamp (BigInteger)"), Category = "Math|BigInteger")
	static FPsDataBigInteger Clamp(FPsDataBigInteger Value, FPsDataBigInteger Min, FPsDataBigInteger Max);

	/** Returns the absolute (positive) value of A */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Absolute (BigInteger)", CompactNodeTitle = "ABS"), Category = "Math|BigInteger")
	static FPsDataBigInteger Abs(FPsDataBigInteger A);

	/** Returns bit value of A */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bit (BigInteger)", CompactNodeTitle = "BIT"), Category = "Math|BigInteger")
	static bool Bit(FPsDataBigInteger A, int32 BitIndex);

	//
	// BigInteger autocast
	//

	/** Converts a BigInteger value to a string */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToShortBigInteger (BigInteger)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|BigInteger")
	static FPsDataShortBigInteger ToShortBigInteger(const FPsDataBigInteger& InBigInteger);

	/** Converts a BigInteger value to a integer */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToInt (BigInteger)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|BigInteger")
	static int32 ToInt(const FPsDataBigInteger& InBigInteger);

	/** Converts a BigInteger value to a integer64 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToInteger64 (BigInteger)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|BigInteger")
	static int32 ToInt64(const FPsDataBigInteger& InBigInteger);

	/** Converts a integer to a BigInteger value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToBigInteger (integer)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|BigInteger")
	static FPsDataBigInteger Int32ToBigInteger(const int32& InInt);

	/** Converts a integer64 to a BigInteger value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToBigInteger (integer64)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|BigInteger")
	static FPsDataBigInteger Int64ToBigInteger(const int64& InInt);

	/** Converts a BigInteger value to a string */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (BigInteger)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
	static FString ToString(const FPsDataBigInteger& InBigInteger);

	/** Converts a string to a BigInteger value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToBigInteger (String)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
	static FPsDataBigInteger StringToBigInteger(const FString& InString);

	//
	// BigInteger & int32
	//

	/** Multiplication (A * B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger * integer", CompactNodeTitle = "*", Keywords = "* multiply"), Category = "Math|BigInteger")
	static FPsDataBigInteger Multiply_Int32(FPsDataBigInteger A, int32 B = 1);

	/** Division (A / B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger / integer", CompactNodeTitle = "/", Keywords = "/ divide division"), Category = "Math|BigInteger")
	static FPsDataBigInteger Divide_Int32(FPsDataBigInteger A, int32 B = 1);

	/** Addition (A + B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger + integer", CompactNodeTitle = "+", Keywords = "+ add plus"), Category = "Math|BigInteger")
	static FPsDataBigInteger Add_Int32(FPsDataBigInteger A, int32 B = 1);

	/** Subtraction (A - B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger - integer", CompactNodeTitle = "-", Keywords = "- subtract minus"), Category = "Math|BigInteger")
	static FPsDataBigInteger Subtract_Int32(FPsDataBigInteger A, int32 B = 1);

	/** Modulo (A % B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger % integer", CompactNodeTitle = "%", Keywords = "% modulus"), Category = "Math|Integer")
	static int32 Modulo_Int32(FPsDataBigInteger A, int32 B = 1);

	/** Returns true if A is less than B (A < B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger < integer", CompactNodeTitle = "<", Keywords = "< less"), Category = "Math|BigInteger")
	static bool Less_Int32(FPsDataBigInteger A, int32 B);

	/** Returns true if A is greater than B (A > B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger > integer", CompactNodeTitle = ">", Keywords = "> greater"), Category = "Math|BigInteger")
	static bool Greater_Int32(FPsDataBigInteger A, int32 B);

	/** Returns true if A is less than or equal to B (A <= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger <= integer", CompactNodeTitle = "<=", Keywords = "<= less"), Category = "Math|BigInteger")
	static bool LessEqual_Int32(FPsDataBigInteger A, int32 B);

	/** Returns true if A is greater than or equal to B (A >= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger >= integer", CompactNodeTitle = ">=", Keywords = ">= greater"), Category = "Math|BigInteger")
	static bool GreaterEqual_Int32(FPsDataBigInteger A, int32 B);

	/** Returns true if A is equal to B (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (BigInteger)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math|BigInteger")
	static bool Equal_Int32(FPsDataBigInteger A, int32 B);

	/** Returns true if A is not equal to B (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (BigInteger)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "Math|BigInteger")
	static bool NotEqual_Int32(FPsDataBigInteger A, int32 B);

	/** Bitwise AND (A & B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise AND", CompactNodeTitle = "&", Keywords = "& and"), Category = "Math|BigInteger")
	static FPsDataBigInteger And_Int32(FPsDataBigInteger A, int32 B);

	/** Bitwise XOR (A ^ B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise XOR", CompactNodeTitle = "^", Keywords = "^ xor"), Category = "Math|BigInteger")
	static FPsDataBigInteger Xor_Int32(FPsDataBigInteger A, int32 B);

	/** Bitwise OR (A | B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise OR", CompactNodeTitle = "|", Keywords = "| or"), Category = "Math|BigInteger")
	static FPsDataBigInteger Or_Int32(FPsDataBigInteger A, int32 B);

	//
	// BigInteger & int64
	//

	/** Multiplication (A * B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger * integer64", CompactNodeTitle = "*", Keywords = "* multiply"), Category = "Math|BigInteger")
	static FPsDataBigInteger Multiply_Int64(FPsDataBigInteger A, int64 B = 1);

	/** Division (A / B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger / integer64", CompactNodeTitle = "/", Keywords = "/ divide division"), Category = "Math|BigInteger")
	static FPsDataBigInteger Divide_Int64(FPsDataBigInteger A, int64 B = 1);

	/** Addition (A + B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger + integer64", CompactNodeTitle = "+", Keywords = "+ add plus"), Category = "Math|BigInteger")
	static FPsDataBigInteger Add_Int64(FPsDataBigInteger A, int64 B = 1);

	/** Subtraction (A - B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger - integer64", CompactNodeTitle = "-", Keywords = "- subtract minus"), Category = "Math|BigInteger")
	static FPsDataBigInteger Subtract_Int64(FPsDataBigInteger A, int64 B = 1);

	/** Modulo (A % B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger % integer64", CompactNodeTitle = "%", Keywords = "% modulus"), Category = "Math|Integer")
	static int64 Modulo_Int64(FPsDataBigInteger A, int64 B = 1);

	/** Returns true if A is less than B (A < B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger < integer64", CompactNodeTitle = "<", Keywords = "< less"), Category = "Math|BigInteger")
	static bool Less_Int64(FPsDataBigInteger A, int64 B);

	/** Returns true if A is greater than B (A > B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger > integer64", CompactNodeTitle = ">", Keywords = "> greater"), Category = "Math|BigInteger")
	static bool Greater_Int64(FPsDataBigInteger A, int64 B);

	/** Returns true if A is less than or equal to B (A <= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger <= integer64", CompactNodeTitle = "<=", Keywords = "<= less"), Category = "Math|BigInteger")
	static bool LessEqual_Int64(FPsDataBigInteger A, int64 B);

	/** Returns true if A is greater than or equal to B (A >= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "BigInteger >= integer64", CompactNodeTitle = ">=", Keywords = ">= greater"), Category = "Math|BigInteger")
	static bool GreaterEqual_Int64(FPsDataBigInteger A, int64 B);

	/** Returns true if A is equal to B (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (BigInteger)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math|BigInteger")
	static bool Equal_Int64(FPsDataBigInteger A, int64 B);

	/** Returns true if A is not equal to B (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (BigInteger)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "Math|BigInteger")
	static bool NotEqual_Int64(FPsDataBigInteger A, int64 B);

	/** Bitwise AND (A & B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise AND", CompactNodeTitle = "&", Keywords = "& and"), Category = "Math|BigInteger")
	static FPsDataBigInteger And_Int64(FPsDataBigInteger A, int64 B);

	/** Bitwise XOR (A ^ B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise XOR", CompactNodeTitle = "^", Keywords = "^ xor"), Category = "Math|BigInteger")
	static FPsDataBigInteger Xor_Int64(FPsDataBigInteger A, int64 B);

	/** Bitwise OR (A | B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Bitwise OR", CompactNodeTitle = "|", Keywords = "| or"), Category = "Math|BigInteger")
	static FPsDataBigInteger Or_Int64(FPsDataBigInteger A, int64 B);

	//
	// PsData functions
	//

	/** Get FPsDataBigInteger property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static FPsDataBigInteger GetBigIntegerProperty(UPsData* Target, int32 Crc32);

	/** Set FPsDataBigInteger property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetBigIntegerProperty(UPsData* Target, int32 Crc32, FPsDataBigInteger Value);

	/** Get FPsDataBigInteger array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<FPsDataBigInteger>& GetBigIntegerArrayProperty(UPsData* Target, int32 Crc32);

	/** Set FPsDataBigInteger array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetBigIntegerArrayProperty(UPsData* Target, int32 Crc32, const TArray<FPsDataBigInteger>& Value);

	/** Get FPsDataBigInteger map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, FPsDataBigInteger>& GetBigIntegerMapProperty(UPsData* Target, int32 Crc32);

	/** Set FPsDataBigInteger map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetBigIntegerMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FPsDataBigInteger>& Value);
};

//
// PsData context
//

template <>
struct FDataTypeContext<FPsDataBigInteger> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataBigIntegerLibrary, FPsDataBigInteger, BigInteger);
};

template <>
struct FDataTypeContext<TArray<FPsDataBigInteger>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataBigIntegerLibrary, TArray<FPsDataBigInteger>, BigIntegerArray);
};

template <>
struct FDataTypeContext<TMap<FString, FPsDataBigInteger>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataBigIntegerLibrary, TMap<FString COMMA FPsDataBigInteger>, BigIntegerMap);
};

//
// PsData Serialize/Deserialize
//

namespace FDataReflectionTools
{
template <>
struct FTypeSerializer<FPsDataBigInteger>
{
	static void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FPsDataBigInteger& Value)
	{
		Serializer->WriteValue(Value.ToString());
	}
};

template <>
struct FTypeDeserializer<FPsDataBigInteger>
{
	static FPsDataBigInteger Deserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)
	{
		FString StringValue;
		int64 Int64Value = 0;
		int32 Int32Value = 0;
		if (Deserializer->ReadValue(StringValue))
		{
			return FPsDataBigInteger(StringValue);
		}
		else if (Deserializer->ReadValue(Int64Value))
		{
			return FPsDataBigInteger(Int64Value);
		}
		else if (Deserializer->ReadValue(Int32Value))
		{
			return FPsDataBigInteger(Int32Value);
		}
		else
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *FDataReflectionTools::FType<FPsDataBigInteger>::Type());
		}
		return FPsDataBigInteger(0);
	}

	static FPsDataBigInteger Deserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FPsDataBigInteger& Value)
	{
		return Deserialize(Instance, Field, Deserializer);
	}
};
} // namespace FDataReflectionTools
