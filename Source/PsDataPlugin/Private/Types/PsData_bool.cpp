// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_bool.h"

DEFINE_FUNCTION(UPsDataBoolLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, bool, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TMap<FString, bool>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBoolLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, bool, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, bool>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBoolLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(bool, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TArray<bool>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBoolLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(bool, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<bool>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBoolLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_UBOOL(Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<bool>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataBoolLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_UBOOL_REF(Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	bool* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataBoolLibrary::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const bool& Value)
{
	Serializer->WriteValue(Value);
}

bool UPsDataBoolLibrary::TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const bool& Value)
{
	bool Result = Value;
	if (Deserializer->ReadValue(Result))
	{
		return Result;
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<bool>::Type());
	return Value;
}