// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_int32.h"

DEFINE_FUNCTION(UPsDataInt32Library::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, int32, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<TMap<FString, int32>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataInt32Library::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, int32, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, int32>* Result = nullptr;
	FDataReflectionTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataInt32Library::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(int32, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<TArray<int32>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataInt32Library::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(int32, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<int32>* Result = nullptr;
	FDataReflectionTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataInt32Library::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_PROPERTY(FIntProperty, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<int32>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataInt32Library::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_PROPERTY_REF(FIntProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	int32* Result = nullptr;
	FDataReflectionTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataInt32Library::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const int32& Value)
{
	Serializer->WriteValue(Value);
}

int32 UPsDataInt32Library::TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const int32& Value)
{
	int32 Result = Value;
	if (Deserializer->ReadValue(Result))
	{
		return Result;
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *FDataReflectionTools::FType<int32>::Type());
	return Value;
}