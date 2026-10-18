// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Types/PsData_FPsDataJson.h"

#include "PsDataDefines.h"
#include "Serialize/PsDataFastJsonSerialization.h"

FPsDataJson::FPsDataJson()
	: JsonString(TEXT("null"))
{
}

FPsDataJson::FPsDataJson(const FString& InJsonString)
	: JsonString(InJsonString.IsEmpty() ? FString(TEXT("null")) : InJsonString)
{
}

const FString& FPsDataJson::GetJsonString() const
{
	// Only a struct that bypassed the constructors (memset, a zeroed buffer) holds an empty string; it means null
	static const FString Null(TEXT("null"));
	return JsonString.IsEmpty() ? Null : JsonString;
}

bool FPsDataJson::IsNull() const
{
	return GetJsonString().Equals(TEXT("null"), ESearchCase::IgnoreCase);
}

void FPsDataJson::Reset()
{
	JsonString = TEXT("null");
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FPsDataJson, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FPsDataJson>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FPsDataJson, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FPsDataJson>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FPsDataJson, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FPsDataJson>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FPsDataJson, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FPsDataJson>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FPsDataJson, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FPsDataJson>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FPsDataJson, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FPsDataJson* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FPsDataJson, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FPsDataJson* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFPsDataJsonLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FPsDataJson, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FPsDataJson>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFPsDataJsonLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FPsDataJson& Value)
{
	// The stored text is parsed and replayed into Serializer, so the value lands as JSON, not as a string
	FPsDataFastJsonDeserializer Deserializer(Value.GetJsonString());
	if (!Deserializer.Translate(Serializer))
	{
		UE_LOG(LogData, Error, TEXT("Can't serialize \"%s::%s\": the stored value is not valid JSON: %s"), *Instance->GetClass()->GetName(), *Field->Name, *Value.GetJsonString());
	}
}

FPsDataJson UPsDataFPsDataJsonLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FPsDataJson& Value)
{
	FPsDataFastJsonSerializer Serializer(false, 256);
	if (Deserializer->Translate(&Serializer))
	{
		return FPsDataJson(Serializer.GetJsonString());
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FPsDataJson>::Type());
	return Value;
}
