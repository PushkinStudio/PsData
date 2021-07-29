// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_Enum.h"

DEFINE_FUNCTION(UPsDataEnumLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, uint8, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, uint8>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, uint8, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, uint8>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(uint8, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<uint8>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(uint8, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<uint8>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY(FByteProperty, Z_Param_Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<uint8>(Target, Index, Z_Param_Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FByteProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	uint8* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataEnumLibrary::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const uint8& Value)
{
	UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
	if (Enum)
	{
		Serializer->WriteValue(Enum->GetNameStringByValue(static_cast<int64>(Value)));
	}
	else
	{
		Serializer->WriteValue(static_cast<uint8>(Value));
	}
}

uint8 UPsDataEnumLibrary::TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const uint8& Value)
{
	UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
	uint8 Result = 0;
	if (Enum)
	{
		FString String;
		if (Deserializer->ReadValue(String))
		{
			Result = static_cast<uint8>(Enum->GetValueByNameString(String, EGetByNameFlags::None));
		}
		else
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize key \"%s\" for \"%s\""), *Field->Name, *Instance->GetPathFromRoot());
		}
	}
	else
	{
		if (!Deserializer->ReadValue(Result))
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize key \"%s\" for \"%s\""), *Field->Name, *Instance->GetPathFromRoot());
		}
	}
	return Result;
}