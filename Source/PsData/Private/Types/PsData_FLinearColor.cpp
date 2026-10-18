// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Types/PsData_FLinearColor.h"

#include "PsDataDefines.h"

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FLinearColor, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FLinearColor>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FLinearColor>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FLinearColor, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FLinearColor>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FLinearColor>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FLinearColor, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FLinearColor>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FLinearColor* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_STRUCT_REF(FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FLinearColor* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FLinearColor>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

UScriptStruct* GetStruct()
{
	return PsDataTools::FindUScriptStruct<FLinearColor>();
}

void UPsDataFLinearColorLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FLinearColor& Value)
{
	FString ResultString;
	const auto Struct = PsDataTools::FindUScriptStruct<FLinearColor>();
	Struct->ExportText(ResultString, &Value, nullptr, nullptr, PPF_None, nullptr);
	Serializer->WriteValue(ResultString);
}

FLinearColor UPsDataFLinearColorLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FLinearColor& Value)
{
	FLinearColor Result;

	FString StringValue;
	if (Deserializer->ReadValue(StringValue))
	{
		const auto Struct = PsDataTools::FindUScriptStruct<FLinearColor>();
		Struct->ImportText(*StringValue, &Result, nullptr, PPF_None, GLog, Struct->GetPathName());
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FVector>::Type());
	}

	return Result;
}
