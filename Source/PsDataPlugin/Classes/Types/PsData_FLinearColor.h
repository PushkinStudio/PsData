// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_FLinearColor.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATAPLUGIN_API UPsDataFLinearColorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Crc32, FLinearColor& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Crc32, const FLinearColor& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Crc32, TArray<FLinearColor>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Crc32, const TArray<FLinearColor>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Crc32, TMap<FString, FLinearColor>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FLinearColor>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FLinearColor& Value);
	static FLinearColor TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FLinearColor& Value);
};

template <>
struct FDataTypeContext<FLinearColor> : public FDataTypeContextExtended<FLinearColor, UPsDataFLinearColorLibrary>
{
};

template <>
struct FDataTypeContext<TArray<FLinearColor>> : public FDataTypeContextExtended<TArray<FLinearColor>, UPsDataFLinearColorLibrary>
{
};

template <>
struct FDataTypeContext<TMap<FString, FLinearColor>> : public FDataTypeContextExtended<TMap<FString, FLinearColor>, UPsDataFLinearColorLibrary>
{
};

namespace FDataReflectionTools
{
template <>
struct FTypeSerializer<FLinearColor> : public FTypeSerializerExtended<FLinearColor, UPsDataFLinearColorLibrary>
{
};

template <>
struct FTypeDeserializer<FLinearColor> : public FTypeDeserializerExtended<FLinearColor, UPsDataFLinearColorLibrary>
{
};

} // namespace FDataReflectionTools