// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_bool.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATAPLUGIN_API UPsDataBoolLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Crc32, bool& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Crc32, bool Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Crc32, TArray<bool>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Crc32, const TArray<bool>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Crc32, TMap<FString, bool>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, bool>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const bool& Value);
	static bool TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const bool& Value);
};

template <>
struct FDataTypeContext<bool> : public FDataTypeContextExtended<bool, UPsDataBoolLibrary>
{
};

template <>
struct FDataTypeContext<TArray<bool>> : public FDataTypeContextExtended<TArray<bool>, UPsDataBoolLibrary>
{
};

template <>
struct FDataTypeContext<TMap<FString, bool>> : public FDataTypeContextExtended<TMap<FString, bool>, UPsDataBoolLibrary>
{
};

namespace FDataReflectionTools
{
template <>
struct FTypeSerializer<bool> : public FTypeSerializerExtended<bool, UPsDataBoolLibrary>
{
};

template <>
struct FTypeDeserializer<bool> : public FTypeDeserializerExtended<bool, UPsDataBoolLibrary>
{
};

} // namespace FDataReflectionTools