// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_Enum.h"

TMap<UEnum*, TMap<uint8, FString>> UPsDataEnumLibrary::EnumValueToString;
TMap<UEnum*, TMap<FString, uint8>> UPsDataEnumLibrary::EnumStringToValue;

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

void UPsDataEnumLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const uint8& Value)
{
	UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
	if (Enum)
	{
		auto Map = EnumValueToString.Find(Enum);
		if (!Map)
		{
			Map = &EnumValueToString.Add(Enum);
			for (int32 i = 0; i < Enum->NumEnums(); ++i)
			{
				Map->Add(static_cast<uint8>(Enum->GetValueByIndex(i)), Enum->GetNameStringByIndex(i));
			}
		}

		if (auto StringValue = Map->Find(Value))
		{
			Serializer->WriteValue(*StringValue);
		}
		else
		{
			Serializer->WriteValue(Value);
		}
	}
	else
	{
		Serializer->WriteValue(Value);
	}
}

uint8 UPsDataEnumLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const uint8& Value)
{
	UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
	uint8 Result = 0;
	if (Enum)
	{
		auto Map = EnumStringToValue.Find(Enum);
		if (!Map)
		{
			Map = &EnumStringToValue.Add(Enum);
			for (int32 i = 0; i < Enum->NumEnums(); ++i)
			{
				Map->Add(Enum->GetNameStringByIndex(i), static_cast<uint8>(Enum->GetValueByIndex(i)));
			}
		}

		FString String;
		if (Deserializer->ReadValue(String))
		{
			if (auto UintValue = Map->Find(String))
			{
				Result = *UintValue;
			}
			else
			{
				UE_LOG(LogData, Warning, TEXT("Can't deserialize key \"%s\" for \"%s\""), *Field->Name, *Instance->GetPathFromRoot());
			}
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
