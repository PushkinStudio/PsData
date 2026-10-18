// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_FIntVector.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataFIntVectorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, FIntVector& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const FIntVector& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<FIntVector>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<FIntVector>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, FIntVector>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, FIntVector>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, FIntVector& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<FIntVector>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FIntVector& Value);
	static FIntVector TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FIntVector& Value);
};

namespace PsDataTools
{
template <>
struct TTypeDefault<FIntVector>
{
	static const FIntVector GetDefaultValue() { return FIntVector::ZeroValue; }
};

template <>
struct TDataTypeContext<FIntVector> : public TDataTypeContextExtended<FIntVector, UPsDataFIntVectorLibrary>
{
};

template <>
struct TDataTypeContext<TArray<FIntVector>> : public TDataTypeContextExtended<TArray<FIntVector>, UPsDataFIntVectorLibrary>
{
};

template <>
struct TDataTypeContext<TMap<FString, FIntVector>> : public TDataTypeContextExtended<TMap<FString, FIntVector>, UPsDataFIntVectorLibrary>
{
};

template <>
struct TTypeSerializer<FIntVector> : public TTypeSerializerExtended<FIntVector, UPsDataFIntVectorLibrary>
{
};

template <>
struct TTypeDeserializer<FIntVector> : public TTypeDeserializerExtended<FIntVector, UPsDataFIntVectorLibrary>
{
};

} // namespace PsDataTools