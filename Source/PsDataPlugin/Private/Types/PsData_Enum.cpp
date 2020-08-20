// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_Enum.h"

DEFINE_FUNCTION(UPsDataEnumLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Z_Param_Target);
	P_GET_PROPERTY(FIntProperty, Z_Param_Crc32);
	P_GET_TMAP_REF(FString, uint8, Z_Param_Out_Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<TMap<FString, uint8>>(Z_Param_Target, Z_Param_Crc32, Z_Param_Out_Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Z_Param_Target);
	P_GET_PROPERTY(FIntProperty, Z_Param_Crc32);
	P_GET_TMAP_REF(FString, uint8, Z_Param_Out_Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, uint8>* Result = nullptr;
	FDataReflectionTools::GetByHash(Z_Param_Target, Z_Param_Crc32, Result);
	Z_Param_Out_Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Z_Param_Target);
	P_GET_PROPERTY(FIntProperty, Z_Param_Crc32);
	P_GET_TARRAY_REF(uint8, Z_Param_Out_Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<TArray<uint8>>(Z_Param_Target, Z_Param_Crc32, Z_Param_Out_Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Z_Param_Target);
	P_GET_PROPERTY(FIntProperty, Z_Param_Crc32);
	P_GET_TARRAY_REF(uint8, Z_Param_Out_Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<uint8>* Result = nullptr;
	FDataReflectionTools::GetByHash(Z_Param_Target, Z_Param_Crc32, Result);
	Z_Param_Out_Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Z_Param_Target);
	P_GET_PROPERTY(FIntProperty, Z_Param_Crc32);
	P_GET_PROPERTY(FByteProperty, Z_Param_Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<uint8>(Z_Param_Target, Z_Param_Crc32, Z_Param_Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Z_Param_Target);
	P_GET_PROPERTY(FIntProperty, Z_Param_Crc32);
	P_GET_PROPERTY_REF(FByteProperty, Z_Param_Out_Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	uint8* Result = nullptr;
	FDataReflectionTools::GetByHash(Z_Param_Target, Z_Param_Crc32, Result);
	Z_Param_Out_Out = *Result;
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