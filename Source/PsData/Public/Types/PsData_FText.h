// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/TextProperty.h"

#include "PsData_FText.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataFTextLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, FText& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const FText& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<FText>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<FText>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, FText>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, FText>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FText& Value);
	static FText TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FText& Value);
};

namespace PsDataTools
{

template <>
struct FDataTypeContext<FText> : public FDataTypeContextExtended<FText, UPsDataFTextLibrary>
{
};

template <>
struct FDataTypeContext<TArray<FText>> : public FDataTypeContextExtended<TArray<FText>, UPsDataFTextLibrary>
{
};

template <>
struct FDataTypeContext<TMap<FString, FText>> : public FDataTypeContextExtended<TMap<FString, FText>, UPsDataFTextLibrary>
{
};

template <>
struct FTypeComparator<FText>
{
	static bool Compare(const FText& Value0, const FText& Value1)
	{
		if (Value0.IsFromStringTable() == Value1.IsFromStringTable())
		{
			if (Value0.IsFromStringTable())
			{
				FName TableId0;
				FString Key0;
				FTextInspector::GetTableIdAndKey(Value0, TableId0, Key0);

				FName TableId1;
				FString Key1;
				FTextInspector::GetTableIdAndKey(Value1, TableId1, Key1);

				return TableId0 == TableId1 && Key0 == Key1;
			}
			else
			{
				return Value0.ToString() == Value1.ToString();
			}
		}
		return false;
	}
};

template <>
struct FTypeSerializer<FText> : public FTypeSerializerExtended<FText, UPsDataFTextLibrary>
{
};

template <>
struct FTypeDeserializer<FText> : public FTypeDeserializerExtended<FText, UPsDataFTextLibrary>
{
};

} // namespace PsDataTools