// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_UPsData.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsDataPtr"))
class PSDATA_API UPsDataUPsDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, UPsData*& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, UPsData* Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<UPsData*>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<UPsData*>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, UPsData*>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, UPsData*>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, UPsData*& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<UPsData*>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const void* Value);
	static void* TypeDeserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, void* Value);
	static bool IsA(const FAbstractDataTypeContext* LeftContext, const FAbstractDataTypeContext* RightContext);
};

namespace PsDataTools
{

template <typename T>
struct TDataTypeContext<T*> : public TDataTypeContextExtended<T*, UPsDataUPsDataLibrary>
{
	virtual UField* GetUEType() const override
	{
		return StaticClass<T>();
	}

	virtual bool IsData() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		if (FAbstractDataTypeContext::IsA(RightContext))
			return true;

		if (RightContext->IsContainer())
			return false;

		return UPsDataUPsDataLibrary::IsA(this, RightContext);
	}
};

template <typename T>
struct TDataTypeContext<TArray<T*>> : public TDataTypeContextExtended<TArray<T*>, UPsDataUPsDataLibrary>
{
	virtual UField* GetUEType() const override
	{
		return GetClass<T>();
	}

	virtual bool IsData() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		if (FAbstractDataTypeContext::IsA(RightContext))
			return true;

		if (!RightContext->IsArray())
			return false;

		return UPsDataUPsDataLibrary::IsA(this, RightContext);
	}
};

template <typename T>
struct TDataTypeContext<TMap<FString, T*>> : public TDataTypeContextExtended<TMap<FString, T*>, UPsDataUPsDataLibrary>
{
	virtual UField* GetUEType() const override
	{
		return GetClass<T>();
	}

	virtual bool IsData() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		if (FAbstractDataTypeContext::IsA(RightContext))
			return true;

		if (!RightContext->IsMap())
			return false;

		return UPsDataUPsDataLibrary::IsA(this, RightContext);
	}
};

template <typename T>
struct TTypeSerializer<T*>
{
	static void Serialize(const UPsData* Instance, const FDataField* Field, FPsDataSerializer* Serializer, const T* Value)
	{
		UPsDataUPsDataLibrary::TypeSerialize(Instance, Field, Serializer, Value);
	}
};

template <typename T>
struct TTypeDeserializer<T*>
{
	static T* Deserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, T* Value)
	{
		return static_cast<T*>(UPsDataUPsDataLibrary::TypeDeserialize(Instance, Field, Deserializer, Value));
	}
};
} // namespace PsDataTools
