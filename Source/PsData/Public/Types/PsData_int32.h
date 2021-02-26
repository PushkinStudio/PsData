// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_int32.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataInt32Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, int32& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, int32 Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<int32>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<int32>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, int32>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, int32>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const int32& Value);
	static int32 TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const int32& Value);
};

namespace PsDataTools
{

template <>
struct FDataTypeContext<int32> : public FDataTypeContextExtended<int32, UPsDataInt32Library>
{
};

template <>
struct FDataTypeContext<TArray<int32>> : public FDataTypeContextExtended<TArray<int32>, UPsDataInt32Library>
{
};

template <>
struct FDataTypeContext<TMap<FString, int32>> : public FDataTypeContextExtended<TMap<FString, int32>, UPsDataInt32Library>
{
};

template <>
struct FTypeDefault<int32>
{
	static const int32 GetDefaultValue() { return 0; }
};

template <>
struct FTypeSerializer<int32> : public FTypeSerializerExtended<int32, UPsDataInt32Library>
{
};

template <>
struct FTypeDeserializer<int32> : public FTypeDeserializerExtended<int32, UPsDataInt32Library>
{
};

} // namespace PsDataTools