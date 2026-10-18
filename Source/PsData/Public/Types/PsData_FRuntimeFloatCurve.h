// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_FRuntimeFloatCurve.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataFRuntimeFloatCurveLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, FRuntimeFloatCurve& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const FRuntimeFloatCurve& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<FRuntimeFloatCurve>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<FRuntimeFloatCurve>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, FRuntimeFloatCurve>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, FRuntimeFloatCurve>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, FRuntimeFloatCurve& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<FRuntimeFloatCurve>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FRuntimeFloatCurve& Value);
	static FRuntimeFloatCurve TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FRuntimeFloatCurve& Value);
};

namespace PsDataTools
{
template <>
struct TDataTypeContext<FRuntimeFloatCurve> : public TDataTypeContextExtended<FRuntimeFloatCurve, UPsDataFRuntimeFloatCurveLibrary>
{
};

template <>
struct TDataTypeContext<TArray<FRuntimeFloatCurve>> : public TDataTypeContextExtended<TArray<FRuntimeFloatCurve>, UPsDataFRuntimeFloatCurveLibrary>
{
};

template <>
struct TDataTypeContext<TMap<FString, FRuntimeFloatCurve>> : public TDataTypeContextExtended<TMap<FString, FRuntimeFloatCurve>, UPsDataFRuntimeFloatCurveLibrary>
{
};

template <>
struct TTypeSerializer<FRuntimeFloatCurve> : public TTypeSerializerExtended<FRuntimeFloatCurve, UPsDataFRuntimeFloatCurveLibrary>
{
};

template <>
struct TTypeDeserializer<FRuntimeFloatCurve> : public TTypeDeserializerExtended<FRuntimeFloatCurve, UPsDataFRuntimeFloatCurveLibrary>
{
};

template <>
struct TTypeComparator<FRuntimeFloatCurve>
{
	static bool Compare(const FRuntimeFloatCurve& Value0, const FRuntimeFloatCurve& Value1)
	{
		return false;
	}
};

} // namespace PsDataTools