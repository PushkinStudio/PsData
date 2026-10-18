// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Types/PsData_FIntVector.h"

#include "PsDataDefines.h"

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FIntVector, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FIntVector>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FIntVector, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FIntVector>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FIntVector, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FIntVector>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FIntVector, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FIntVector>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FIntVector, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FIntVector>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FIntVector, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FIntVector* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FIntVector, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FIntVector* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFIntVectorLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FIntVector, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FIntVector>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFIntVectorLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FIntVector& Value)
{
	FString ResultString;
	const auto Struct = PsDataTools::FindUScriptStruct<FIntVector>();
	Struct->ExportText(ResultString, &Value, nullptr, nullptr, PPF_None, nullptr);
	Serializer->WriteValue(ResultString);
}

FIntVector UPsDataFIntVectorLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FIntVector& Value)
{
	auto Result = FIntVector::ZeroValue;

	FString StringValue;
	if (Deserializer->ReadValue(StringValue))
	{
		const auto Struct = PsDataTools::FindUScriptStruct<FIntVector>();
		Struct->ImportText(*StringValue, &Result, nullptr, PPF_None, GLog, Struct->GetPathName());
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FIntVector>::Type());
	}

	return Result;
}