// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Types/PsData_Enum.h"

#include "PsDataDefines.h"
#include "Types/PsData_uint8.h"

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

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FByteProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	uint8* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataEnumLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(uint8, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<uint8>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataEnumLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const uint8& Value)
{
	UEnum* Enum = Cast<UEnum>(Field->Context->GetUEType());
	if (Enum)
	{
		if (const auto StringValue = GetEnumString(Enum, Value))
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
	UEnum* Enum = Cast<UEnum>(Field->Context->GetUEType());
	uint8 Result = 0;
	if (Enum)
	{
		FString String;
		if (Deserializer->ReadValue(String))
		{
			if (const auto UintValue = GetEnumValue(Enum, String))
			{
				Result = *UintValue;
				return Result;
			}
		}
	}

	if (Deserializer->ReadValue(Result))
	{
		return Result;
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *Field->Context->GetCppType());
	return Result;
}

FString* UPsDataEnumLibrary::GetEnumString(UEnum* Enum, uint8 Value)
{
	auto ValueToStringPtr = EnumValueToString.Find(Enum);
	if (!ValueToStringPtr)
	{
		ValueToStringPtr = &EnumValueToString.Add(Enum);
		for (int32 i = 0; i < Enum->NumEnums(); ++i)
		{
			ValueToStringPtr->Add(static_cast<uint8>(Enum->GetValueByIndex(i)), Enum->GetNameStringByIndex(i));
		}
	}

	return ValueToStringPtr->Find(Value);
}

uint8* UPsDataEnumLibrary::GetEnumValue(UEnum* Enum, const FString& String)
{
	auto StringToValuePtr = EnumStringToValue.Find(Enum);
	if (!StringToValuePtr)
	{
		StringToValuePtr = &EnumStringToValue.Add(Enum);
		for (int32 i = 0; i < Enum->NumEnums(); ++i)
		{
			StringToValuePtr->Add(Enum->GetNameStringByIndex(i), static_cast<uint8>(Enum->GetValueByIndex(i)));
		}
	}

	return StringToValuePtr->Find(String);
}
