// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

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

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Hash, TSoftClassPtr<UObject>& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Hash, const TSoftClassPtr<UObject>& Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Hash, TArray<TSoftClassPtr<UObject>>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Hash, const TArray<TSoftClassPtr<UObject>>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Hash, TMap<FString, TSoftClassPtr<UObject>>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Hash", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Hash, const TMap<FString, TSoftClassPtr<UObject>>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FSoftObjectPath& Value);
	static FSoftObjectPath TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FSoftObjectPath& Value);
};

namespace PsDataTools
{

template <typename T>
struct FDataTypeContext<TSoftClassPtr<T>> : public FDataTypeContextExtended<TSoftClassPtr<T>, UPsDataTSoftClassPtrLibrary>
{
	virtual UField* GetUE4Type() const override
	{
		return GetClass<T>();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}
};

template <typename T>
struct FDataTypeContext<TArray<TSoftClassPtr<T>>> : public FDataTypeContextExtended<TArray<TSoftClassPtr<T>>, UPsDataTSoftClassPtrLibrary>
{
	virtual UField* GetUE4Type() const override
	{
		return GetClass<T>();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}
};

template <typename T>
struct FDataTypeContext<TMap<FString, TSoftClassPtr<T>>> : public FDataTypeContextExtended<TMap<FString, TSoftClassPtr<T>>, UPsDataTSoftClassPtrLibrary>
{
	virtual UField* GetUE4Type() const override
	{
		return GetClass<T>();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}
};

template <typename T>
struct FTypeSerializer<TSoftClassPtr<T>>
{
	static void Serialize(const UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const TSoftClassPtr<T>& Value)
	{
		UPsDataTSoftClassPtrLibrary::TypeSerialize(Instance, Field, Serializer, Value.GetUniqueID());
	}
};

template <typename T>
struct FTypeDeserializer<TSoftClassPtr<T>>
{
	static TSoftClassPtr<T> Deserialize(const UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const TSoftClassPtr<T>& Value)
	{
		return TSoftClassPtr<T>(UPsDataTSoftClassPtrLibrary::TypeDeserialize(Instance, Field, Deserializer, Value.GetUniqueID()));
	}
};

} // namespace PsDataTools