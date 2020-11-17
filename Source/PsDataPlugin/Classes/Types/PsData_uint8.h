// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_uint8.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATAPLUGIN_API UPsDataUint8Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Hash, uint8& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Hash, uint8 Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Hash, TArray<uint8>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Hash, const TArray<uint8>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Hash, TMap<FString, uint8>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Hash, const TMap<FString, uint8>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const uint8& Value);
	static uint8 TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const uint8& Value);
};

namespace PsDataTools
{

template <>
struct FDataTypeContext<uint8> : public FDataTypeContextExtended<uint8, UPsDataUint8Library>
{
};

template <>
struct FDataTypeContext<TArray<uint8>> : public FDataTypeContextExtended<TArray<uint8>, UPsDataUint8Library>
{
};

template <>
struct FDataTypeContext<TMap<FString, uint8>> : public FDataTypeContextExtended<TMap<FString, uint8>, UPsDataUint8Library>
{
};

template <>
struct FTypeDefault<uint8>
{
	static const uint8 GetDefaultValue() { return 0; }
};

template <>
struct FTypeSerializer<uint8> : public FTypeSerializerExtended<uint8, UPsDataUint8Library>
{
};

template <>
struct FTypeDeserializer<uint8> : public FTypeDeserializerExtended<uint8, UPsDataUint8Library>
{
};

} // namespace PsDataTools