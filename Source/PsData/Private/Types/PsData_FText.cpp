// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_FText.h"

DEFINE_FUNCTION(UPsDataFTextLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, FText, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TMap<FString, FText>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, FText, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FText>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(FText, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TArray<FText>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(FText, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FText>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_PROPERTY_REF(FTextProperty, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<FText>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_PROPERTY_REF(FTextProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FText* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFTextLibrary::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FText& Value)
{
	static const FString TableIdParam(TEXT("TableId"));
	static const FString KeyParam(TEXT("Key"));

	if (Value.IsFromStringTable())
	{
		FName TableIdValue;
		FString KeyValue;
		FTextInspector::GetTableIdAndKey(Value, TableIdValue, KeyValue);

		Serializer->WriteObject();
		Serializer->WriteKey(TableIdParam);
		Serializer->WriteValue(TableIdValue);
		Serializer->PopKey(TableIdParam);
		Serializer->WriteKey(KeyParam);
		Serializer->WriteValue(KeyValue);
		Serializer->PopKey(KeyParam);
		Serializer->PopObject();
	}
	else
	{
		Serializer->WriteValue(Value.ToString());
	}
}

FText UPsDataFTextLibrary::TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FText& Value)
{
	static const FString TableIdParam(TEXT("TableId"));
	static const FString KeyParam(TEXT("Key"));

	if (Deserializer->ReadObject())
	{
		FName TableIdValue;
		FString KeyValue;

		FString Key;
		while (Deserializer->ReadKey(Key))
		{
			if (Key == TableIdParam)
			{
				Deserializer->ReadValue(TableIdValue);
			}
			else if (Key == KeyParam)
			{
				Deserializer->ReadValue(KeyValue);
			}
			Deserializer->PopKey(Key);
		}
		Deserializer->PopObject();

		if (TableIdValue == NAME_None || KeyValue.IsEmpty())
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\" (Object must have fields: \"%s\" and \"%s\")"), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FText>::Type(), *TableIdParam, *KeyParam)
		}

		return FText::FromStringTable(TableIdValue, KeyValue);
	}
	else
	{
		FString String;
		if (!Deserializer->ReadValue(String))
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FText>::Type())
		}

		return FText::FromString(String);
	}
}