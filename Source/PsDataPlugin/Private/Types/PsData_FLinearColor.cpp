// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_FLinearColor.h"

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, FLinearColor, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<TMap<FString, FLinearColor>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, FLinearColor>* Result = nullptr;
	FDataReflectionTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(FLinearColor, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<TArray<FLinearColor>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<FLinearColor>* Result = nullptr;
	FDataReflectionTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_STRUCT_REF(FLinearColor, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	FDataReflectionTools::SetByHash<FLinearColor>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataFLinearColorLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_STRUCT_REF(FLinearColor, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	FLinearColor* Result = nullptr;
	FDataReflectionTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataFLinearColorLibrary::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const FLinearColor& Value)
{
	static const FString RParam(TEXT("r"));
	static const FString GParam(TEXT("g"));
	static const FString BParam(TEXT("b"));
	static const FString AParam(TEXT("a"));

	Serializer->WriteObject();
	Serializer->WriteKey(RParam);
	Serializer->WriteValue(Value.R);
	Serializer->PopKey(RParam);
	Serializer->WriteKey(GParam);
	Serializer->WriteValue(Value.G);
	Serializer->PopKey(GParam);
	Serializer->WriteKey(BParam);
	Serializer->WriteValue(Value.B);
	Serializer->PopKey(BParam);
	Serializer->WriteKey(AParam);
	Serializer->WriteValue(Value.A);
	Serializer->PopKey(AParam);
	Serializer->PopObject();
}

FLinearColor UPsDataFLinearColorLibrary::TypeDeserialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, const FLinearColor& Value)
{
	static const FString RParam(TEXT("r"));
	static const FString GParam(TEXT("g"));
	static const FString BParam(TEXT("b"));
	static const FString AParam(TEXT("a"));

	FLinearColor Result;
	if (Deserializer->ReadObject())
	{
		FString Key;
		while (Deserializer->ReadKey(Key))
		{
			float Channel = 0;
			if (Key == RParam && Deserializer->ReadValue(Channel))
			{
				Result.R = Channel;
			}
			else if (Key == GParam && Deserializer->ReadValue(Channel))
			{
				Result.G = Channel;
			}
			else if (Key == BParam && Deserializer->ReadValue(Channel))
			{
				Result.B = Channel;
			}
			else if (Key == AParam && Deserializer->ReadValue(Channel))
			{
				Result.A = Channel;
			}
			Deserializer->PopKey(Key);
		}
		Deserializer->PopObject();
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *FDataReflectionTools::FType<FLinearColor>::Type());
	}
	return Result;
}