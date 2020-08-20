// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_FString.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATAPLUGIN_API UPsDataFStringLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Crc32, FString& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Crc32, const FString& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Crc32, TArray<FString>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Crc32, const TArray<FString>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Crc32, TMap<FString, FString>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FString>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FString& Value);
	static FString TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FString& Value);
};

template <>
struct FDataTypeContext<FString> : public FDataTypeContextExtended<FString, UPsDataFStringLibrary>
{
};

template <>
struct FDataTypeContext<TArray<FString>> : public FDataTypeContextExtended<TArray<FString>, UPsDataFStringLibrary>
{
};

template <>
struct FDataTypeContext<TMap<FString, FString>> : public FDataTypeContextExtended<TMap<FString, FString>, UPsDataFStringLibrary>
{
};

namespace FDataReflectionTools
{
template <>
struct FTypeSerializer<FString> : public FTypeSerializerExtended<FString, UPsDataFStringLibrary>
{
};

template <>
struct FTypeDeserializer<FString> : public FTypeDeserializerExtended<FString, UPsDataFStringLibrary>
{
};

} // namespace FDataReflectionTools