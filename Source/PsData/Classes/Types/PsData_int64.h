// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_int64.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATAPLUGIN_API UPsDataInt64Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Hash, int64& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Hash, int64 Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Hash, TArray<int64>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Hash, const TArray<int64>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Hash, TMap<FString, int64>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Hash, const TMap<FString, int64>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const int64& Value);
	static int64 TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const int64& Value);
};

namespace PsDataTools
{

template <>
struct FDataTypeContext<int64> : public FDataTypeContextExtended<int64, UPsDataInt64Library>
{
};

template <>
struct FDataTypeContext<TArray<int64>> : public FDataTypeContextExtended<TArray<int64>, UPsDataInt64Library>
{
};

template <>
struct FDataTypeContext<TMap<FString, int64>> : public FDataTypeContextExtended<TMap<FString, int64>, UPsDataInt64Library>
{
};

template <>
struct FTypeDefault<int64>
{
	static const int64 GetDefaultValue() { return 0; }
};

template <>
struct FTypeSerializer<int64> : public FTypeSerializerExtended<int64, UPsDataInt64Library>
{
};

template <>
struct FTypeDeserializer<int64> : public FTypeDeserializerExtended<int64, UPsDataInt64Library>
{
};

} // namespace PsDataTools