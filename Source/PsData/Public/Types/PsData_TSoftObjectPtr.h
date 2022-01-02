// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_TSoftObjectPtr.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataTSoftObjectPtrLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, TSoftObjectPtr<UObject>& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const TSoftObjectPtr<UObject>& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<TSoftObjectPtr<UObject>>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<TSoftObjectPtr<UObject>>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, TSoftObjectPtr<UObject>>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, TSoftObjectPtr<UObject>>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, TSoftObjectPtr<UObject>& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<TSoftObjectPtr<UObject>>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FSoftObjectPath& Value);
	static FSoftObjectPath TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FSoftObjectPath& Value);
};

namespace PsDataTools
{

template <typename T>
struct TDataTypeContext<TSoftObjectPtr<T>> : public TDataTypeContextExtended<TSoftObjectPtr<T>, UPsDataTSoftObjectPtrLibrary>
{
	virtual UField* GetUEType() const override
	{
		return GetClass<T>();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}
};

template <typename T>
struct TDataTypeContext<TArray<TSoftObjectPtr<T>>> : public TDataTypeContextExtended<TArray<TSoftObjectPtr<T>>, UPsDataTSoftObjectPtrLibrary>
{
	virtual UField* GetUEType() const override
	{
		return GetClass<T>();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}
};

template <typename T>
struct TDataTypeContext<TMap<FString, TSoftObjectPtr<T>>> : public TDataTypeContextExtended<TMap<FString, TSoftObjectPtr<T>>, UPsDataTSoftObjectPtrLibrary>
{
	virtual UField* GetUEType() const override
	{
		return GetClass<T>();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}
};

template <typename T>
struct TTypeSerializer<TSoftObjectPtr<T>>
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const TSoftObjectPtr<T>& Value)
	{
		UPsDataTSoftObjectPtrLibrary::TypeSerialize(Instance, Field, Serializer, Value.GetUniqueID());
	}
};

template <typename T>
struct TTypeDeserializer<TSoftObjectPtr<T>>
{
	static TSoftObjectPtr<T> Deserialize(const UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const TSoftObjectPtr<T>& Value)
	{
		return TSoftObjectPtr<T>(UPsDataTSoftObjectPtrLibrary::TypeDeserialize(Instance, Field, Deserializer, Value.GetUniqueID()));
	}
};

} // namespace PsDataTools