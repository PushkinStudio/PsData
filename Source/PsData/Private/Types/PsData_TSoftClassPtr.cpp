// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Types/PsData_TSoftClassPtr.h"

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, TSoftClassPtr<UObject>, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, TSoftClassPtr<UObject>>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, TSoftClassPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, TSoftClassPtr<UObject>>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(TSoftClassPtr<UObject>, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<TSoftClassPtr<UObject>>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(TSoftClassPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<TSoftClassPtr<UObject>>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_SOFTCLASS_REF(TSoftClassPtr<UObject>, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TSoftClassPtr<UObject>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_SOFTCLASS_REF(TSoftClassPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TSoftClassPtr<UObject>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_SOFTCLASS_REF(TSoftClassPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TSoftClassPtr<UObject>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftClassPtrLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(TSoftClassPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<TSoftClassPtr<UObject>>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataTSoftClassPtrLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FSoftObjectPath& Value)
{
	Serializer->WriteValue(Value.ToString());
}

FSoftObjectPath UPsDataTSoftClassPtrLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FSoftObjectPath& Value)
{
	FString String;
	if (Deserializer->ReadValue(String))
	{
		return FSoftObjectPath(String);
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FSoftObjectPath>::Type());
	return {};
}