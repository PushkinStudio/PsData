// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_UPsData.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATAPLUGIN_API UPsDataUPsDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Crc32, UPsData*& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Crc32, UPsData* Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Crc32, TArray<UPsData*>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Crc32, const TArray<UPsData*>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Crc32, TMap<FString, UPsData*>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataHash = "Crc32", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, UPsData*>& Value);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);

	static void TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const void* Value);
	static void* TypeDeserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, void* Value);
	static bool IsA(const FAbstractDataTypeContext* LeftContext, const FAbstractDataTypeContext* RightContext);
};

template <typename T>
struct FDataTypeContext<T*> : public FDataTypeContextExtended<T*, UPsDataUPsDataLibrary>
{
	virtual UField* GetUE4Type() const override
	{
		return FDataReflectionTools::GetClass<T>();
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
struct FDataTypeContext<TArray<T*>> : public FDataTypeContextExtended<TArray<T*>, UPsDataUPsDataLibrary>
{
	virtual UField* GetUE4Type() const override
	{
		return FDataReflectionTools::GetClass<T>();
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
struct FDataTypeContext<TMap<FString, T*>> : public FDataTypeContextExtended<TMap<FString, T*>, UPsDataUPsDataLibrary>
{
	virtual UField* GetUE4Type() const override
	{
		return FDataReflectionTools::GetClass<T>();
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

namespace FDataReflectionTools
{
template <typename T>
struct FTypeSerializer<T*>
{
	static void Serialize(const UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const T* Value)
	{
		UPsDataUPsDataLibrary::TypeSerialize(Instance, Field, Serializer, Value);
	}
};

template <typename T>
struct FTypeDeserializer<T*>
{
	static T* Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, T* Value)
	{
		return static_cast<T*>(UPsDataUPsDataLibrary::TypeDeserialize(Instance, Field, Deserializer, Value));
	}
};
} // namespace FDataReflectionTools
