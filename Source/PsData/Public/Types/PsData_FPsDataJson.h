// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_FPsDataJson.generated.h"

/**
 * A JSON value of any shape, kept as compact JSON text. On the wire it is the value itself rather than
 * a string: a JSON document embeds it as JSON, binary carries its structure. For payloads whose schema
 * the model does not own — a server notification, an item's free-form attributes.
 */
USTRUCT(BlueprintType)
struct PSDATA_API FPsDataJson
{
	GENERATED_BODY()

	/** null */
	FPsDataJson();

	/** Any JSON text: an object, an array, a string, a number, true, false or null */
	FPsDataJson(const FString& InJsonString);

	/** Compact JSON text, "null" when empty */
	const FString& GetJsonString() const;

	bool IsNull() const;

	/** Back to null */
	void Reset();

private:
	FString JsonString;
};

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataFPsDataJsonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, FPsDataJson& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const FPsDataJson& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<FPsDataJson>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<FPsDataJson>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, FPsDataJson>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, FPsDataJson>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, FPsDataJson& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<FPsDataJson>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FPsDataJson& Value);
	static FPsDataJson TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FPsDataJson& Value);
};

namespace PsDataTools
{

template <>
struct TDataTypeContext<FPsDataJson> : public TDataTypeContextExtended<FPsDataJson, UPsDataFPsDataJsonLibrary>
{
};

template <>
struct TDataTypeContext<TArray<FPsDataJson>> : public TDataTypeContextExtended<TArray<FPsDataJson>, UPsDataFPsDataJsonLibrary>
{
};

template <>
struct TDataTypeContext<TMap<FString, FPsDataJson>> : public TDataTypeContextExtended<TMap<FString, FPsDataJson>, UPsDataFPsDataJsonLibrary>
{
};

template <>
struct TTypeComparator<FPsDataJson>
{
	static bool Compare(const FPsDataJson& Value0, const FPsDataJson& Value1)
	{
		return Value0.GetJsonString().Equals(Value1.GetJsonString(), ESearchCase::CaseSensitive);
	}
};

template <>
struct TTypeSerializer<FPsDataJson> : public TTypeSerializerExtended<FPsDataJson, UPsDataFPsDataJsonLibrary>
{
};

template <>
struct TTypeDeserializer<FPsDataJson> : public TTypeDeserializerExtended<FPsDataJson, UPsDataFPsDataJsonLibrary>
{
};

} // namespace PsDataTools
