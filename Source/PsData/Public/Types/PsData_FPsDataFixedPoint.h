// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"
#include "Types/PsDataFixedPoint.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_FPsDataFixedPoint.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataFixedPointLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//
	// Literals
	//

	/** Make FixedPoint from integer64 */
	UFUNCTION(BlueprintPure, Category = "Math|FixedPoint")
	static FPsDataFixedPoint MakeLiteralFixedPoint(int32 Value);

	/** Make FixedPoint from integer */
	UFUNCTION(BlueprintPure, Category = "Math|FixedPoint")
	static FPsDataFixedPoint MakeLiteralFixedPointFromInt64(int64 Value);

	/** Make FixedPoint from integer */
	UFUNCTION(BlueprintPure, Category = "Math|FixedPoint")
	static FPsDataFixedPoint MakeLiteralFixedPointFromFloat(float Value);

	/** Make FixedPoint from string */
	UFUNCTION(BlueprintPure, Category = "Math|FixedPoint")
	static FPsDataFixedPoint MakeLiteralFixedPointFromString(FString Value);

	//
	// FixedPoint & FixedPoint
	//

	/** Multiplication (A * B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint * FixedPoint", CompactNodeTitle = "*", Keywords = "* multiply", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Multiply(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Division (A / B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint / FixedPoint", CompactNodeTitle = "/", Keywords = "/ divide division"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Divide(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Addition (A + B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint + FixedPoint", CompactNodeTitle = "+", Keywords = "+ add plus", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Add(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Subtraction (A - B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint - FixedPoint", CompactNodeTitle = "-", Keywords = "- subtract minus"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Subtract(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if A is less than B (A < B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint < FixedPoint", CompactNodeTitle = "<", Keywords = "< less"), Category = "Math|FixedPoint")
	static bool Less(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if A is greater than B (A > B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint > FixedPoint", CompactNodeTitle = ">", Keywords = "> greater"), Category = "Math|FixedPoint")
	static bool Greater(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if A is less than or equal to B (A <= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint <= FixedPoint", CompactNodeTitle = "<=", Keywords = "<= less"), Category = "Math|FixedPoint")
	static bool LessEqual(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if A is greater than or equal to B (A >= B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "FixedPoint >= FixedPoint", CompactNodeTitle = ">=", Keywords = ">= greater"), Category = "Math|FixedPoint")
	static bool GreaterEqual(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if A is equal to B (A == B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (FixedPoint)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math|FixedPoint")
	static bool Equal(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if A is not equal to B (A != B) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (FixedPoint)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "Math|FixedPoint")
	static bool NotEqual(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Sign (returns -1 if A < 0, 0 if A is zero, and +1 if A > 0) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sign (FixedPoint)"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Sign(FPsDataFixedPoint A);

	/** Returns the minimum value of A and B */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Min (FixedPoint)", CompactNodeTitle = "MIN", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Min(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns the maximum value of A and B */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Max (FixedPoint)", CompactNodeTitle = "MAX", CommutativeAssociativeBinaryOperator = "true"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Max(FPsDataFixedPoint A, FPsDataFixedPoint B);

	/** Returns true if value is between Min and Max (V >= Min && V <= Max)
	 * If InclusiveMin is true, value needs to be equal or larger than Min, else it needs to be larger
	 * If InclusiveMax is true, value needs to be smaller or equal than Max, else it needs to be smaller
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "InRange (FixedPoint)", Min = "0", Max = "10"), Category = "Math|FixedPoint")
	static bool InRange(FPsDataFixedPoint Value, FPsDataFixedPoint Min, FPsDataFixedPoint Max, bool InclusiveMin = true, bool InclusiveMax = true);

	/** Returns Value clamped to be between A and B (inclusive) */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Clamp (FixedPoint)"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Clamp(FPsDataFixedPoint Value, FPsDataFixedPoint Min, FPsDataFixedPoint Max);

	/** Returns the absolute (positive) value of A */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Absolute (FixedPoint)", CompactNodeTitle = "ABS"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Abs(FPsDataFixedPoint A);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Floor (FixedPoint)", CompactNodeTitle = "Floor"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Floor(FPsDataFixedPoint Value);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Ceil (FixedPoint)", CompactNodeTitle = "Ceil"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Ceil(FPsDataFixedPoint Value);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Round (FixedPoint)", CompactNodeTitle = "Round"), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Round(FPsDataFixedPoint Value);

	//
	// FixedPoint autocast
	//

	/** Converts a FixedPoint value to a integer */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToInt (FixedPoint)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|FixedPoint")
	static int64 ToInt(const FPsDataFixedPoint& InFixedPoint);

	/** Converts a FixedPoint value to a integer64 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFloat (FixedPoint)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|FixedPoint")
	static float ToFloat(const FPsDataFixedPoint& InFixedPoint);

	/** Converts a integer to a FixedPoint value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFixedPoint (integer)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Int32ToFixedPoint(const int32& InInt);

	/** Converts a integer64 to a FixedPoint value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFixedPoint (integer64)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|FixedPoint")
	static FPsDataFixedPoint Int64ToFixedPoint(const int64& InInt);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFixedPoint (float)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Math|FixedPoint")
	static FPsDataFixedPoint FloatToFixedPoint(const float& InFloat);

	/** Converts a FixedPoint value to a string */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (FixedPoint)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
	static FString ToString(const FPsDataFixedPoint& InFixedPoint);

	/** Converts a string to a FixedPoint value */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFixedPoint (String)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
	static FPsDataFixedPoint StringToFixedPoint(const FString& InString);

	//
	// PsData functions
	//

private:
	/** Get FPsDataFixedPoint property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, FPsDataFixedPoint& Out);

	/** Set FPsDataFixedPoint property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const FPsDataFixedPoint& Value);

	/** Get FPsDataFixedPoint array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<FPsDataFixedPoint>& Out);

	/** Set FPsDataFixedPoint array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<FPsDataFixedPoint>& Value);

	/** Get FPsDataFixedPoint map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, FPsDataFixedPoint>& Out);

	/** Set FPsDataFixedPoint map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, FPsDataFixedPoint>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, FPsDataFixedPoint& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<FPsDataFixedPoint>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FPsDataFixedPoint& Value);
	static FPsDataFixedPoint TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FPsDataFixedPoint& Value);
};

namespace PsDataTools
{

template <>
struct TDataTypeContext<FPsDataFixedPoint> : public TDataTypeContextExtended<FPsDataFixedPoint, UPsDataFixedPointLibrary>
{
};

template <>
struct TDataTypeContext<TArray<FPsDataFixedPoint>> : public TDataTypeContextExtended<TArray<FPsDataFixedPoint>, UPsDataFixedPointLibrary>
{
};

template <>
struct TDataTypeContext<TMap<FString, FPsDataFixedPoint>> : public TDataTypeContextExtended<TMap<FString, FPsDataFixedPoint>, UPsDataFixedPointLibrary>
{
};

template <>
struct TTypeSerializer<FPsDataFixedPoint> : public TTypeSerializerExtended<FPsDataFixedPoint, UPsDataFixedPointLibrary>
{
};

template <>
struct TTypeDeserializer<FPsDataFixedPoint> : public TTypeDeserializerExtended<FPsDataFixedPoint, UPsDataFixedPointLibrary>
{
};
} // namespace PsDataTools
