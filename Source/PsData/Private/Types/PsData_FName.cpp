// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Types/PsData_FName.h"

#include "PsDataDefines.h"

DEFINE_FUNCTION(UPsDataFNameLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FName, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FName>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FName, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FName>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FName, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FName>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FName, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FName>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY(FNameProperty, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FName>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FNameProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FName* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FNameProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FName* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFNameLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FName, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FName>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFNameLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FName& Value)
{
	Serializer->WriteValue(Value);
}

FName UPsDataFNameLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FName& Value)
{
	FName Name;
	if (Deserializer->ReadValue(Name))
	{
		return Name;
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FName>::Type())
	return NAME_None;
}
