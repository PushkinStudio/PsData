// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_TSoftClassPtr.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataTSoftClassPtrLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, TSoftClassPtr<UObject>& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, const TSoftClassPtr<UObject>& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<TSoftClassPtr<UObject>>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<TSoftClassPtr<UObject>>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, TSoftClassPtr<UObject>>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, TSoftClassPtr<UObject>>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, TSoftClassPtr<UObject>& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<TSoftClassPtr<UObject>>& Out);

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
struct TDataTypeContext<TSoftClassPtr<T>> : public TDataTypeContextExtended<TSoftClassPtr<T>, UPsDataTSoftClassPtrLibrary>
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
struct TDataTypeContext<TArray<TSoftClassPtr<T>>> : public TDataTypeContextExtended<TArray<TSoftClassPtr<T>>, UPsDataTSoftClassPtrLibrary>
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
struct TDataTypeContext<TMap<FString, TSoftClassPtr<T>>> : public TDataTypeContextExtended<TMap<FString, TSoftClassPtr<T>>, UPsDataTSoftClassPtrLibrary>
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
struct TTypeSerializer<TSoftClassPtr<T>>
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const TSoftClassPtr<T>& Value)
	{
		UPsDataTSoftClassPtrLibrary::TypeSerialize(Instance, Field, Serializer, Value.GetUniqueID());
	}
};

template <typename T>
struct TTypeDeserializer<TSoftClassPtr<T>>
{
	static TSoftClassPtr<T> Deserialize(const UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const TSoftClassPtr<T>& Value)
	{
		return TSoftClassPtr<T>(UPsDataTSoftClassPtrLibrary::TypeDeserialize(Instance, Field, Deserializer, Value.GetUniqueID()));
	}
};

} // namespace PsDataTools