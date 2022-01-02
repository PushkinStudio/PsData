// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Types/PsData_FText.h"

#include "UObject/TextProperty.h"

DEFINE_FUNCTION(UPsDataFTextLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FText, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, FText>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, FText, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FText>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FText, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<FText>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FText, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FText>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FTextProperty, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<FText>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FTextProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FText* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_PROPERTY_REF(FTextProperty, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FText* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFTextLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(FText, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FText>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFTextLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FText& Value)
{
	static const FString TableIdParam(TEXT("TableId"));
	static const FString KeyParam(TEXT("Key"));
	static const FString EmptyParam(TEXT("Empty"));

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
	else if (Value.IsEmpty())
	{
		Serializer->WriteObject();
		Serializer->WriteKey(EmptyParam);
		Serializer->WriteValue(true);
		Serializer->PopKey(EmptyParam);
		Serializer->PopObject();
	}
	else
	{
		Serializer->WriteValue(Value.ToString());
	}
}

FText UPsDataFTextLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FText& Value)
{
	static const FString TableIdParam(TEXT("TableId"));
	static const FString KeyParam(TEXT("Key"));
	static const FString EmptyParam(TEXT("Empty"));

	if (Deserializer->ReadObject())
	{
		FString TableIdValue;
		FString KeyValue;
		bool EmptyValue = false;

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
			else if (Key == EmptyParam)
			{
				Deserializer->ReadValue(EmptyValue);
			}
			Deserializer->PopKey(Key);
		}
		Deserializer->PopObject();

		if (EmptyValue)
		{
			return FText::GetEmpty();
		}
		else if (!TableIdValue.IsEmpty() && !KeyValue.IsEmpty())
		{
			return FText::FromStringTable(*TableIdValue, KeyValue);
		}
	}
	else
	{
		FString String;
		if (Deserializer->ReadValue(String))
		{
			return FText::FromString(String);
		}
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FText>::Type());

	return FText::GetEmpty();
}