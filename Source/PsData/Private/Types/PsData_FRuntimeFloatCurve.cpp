// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Types/PsData_FRuntimeFloatCurve.h"

#include "JsonObjectConverter.h"
#include "PsDataDefines.h"
#include "Serialize/PsDataStructSerialization.h"

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FRuntimeFloatCurve, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FRuntimeFloatCurve>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FRuntimeFloatCurve, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FRuntimeFloatCurve>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FRuntimeFloatCurve, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FRuntimeFloatCurve>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FRuntimeFloatCurve, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FRuntimeFloatCurve>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FRuntimeFloatCurve, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FRuntimeFloatCurve>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FRuntimeFloatCurve, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FRuntimeFloatCurve* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FRuntimeFloatCurve, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FRuntimeFloatCurve* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFRuntimeFloatCurveLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FRuntimeFloatCurve, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FRuntimeFloatCurve>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFRuntimeFloatCurveLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FRuntimeFloatCurve& Value)
{
	FString ResultString;
	const auto Struct = FRuntimeFloatCurve::StaticStruct();
	Struct->ExportText(ResultString, &Value, nullptr, nullptr, PPF_None, nullptr);
	Serializer->WriteValue(ResultString);
}

FRuntimeFloatCurve UPsDataFRuntimeFloatCurveLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FRuntimeFloatCurve& Value)
{
	FRuntimeFloatCurve Result;

	FString StringValue;
	if (Deserializer->ReadValue(StringValue))
	{
		const auto Struct = FRuntimeFloatCurve::StaticStruct();
		Struct->ImportText(*StringValue, &Result, nullptr, PPF_None, GLog, Struct->GetPathName());
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FRuntimeFloatCurve>::Type());
	}

	return Result;
}