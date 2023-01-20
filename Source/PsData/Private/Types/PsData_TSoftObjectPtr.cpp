// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Types/PsData_TSoftObjectPtr.h"

#include "PsDataDefines.h"

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, TSoftObjectPtr<UObject>, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, TSoftObjectPtr<UObject>>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, TSoftObjectPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, TSoftObjectPtr<UObject>>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(TSoftObjectPtr<UObject>, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<TSoftObjectPtr<UObject>>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(TSoftObjectPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<TSoftObjectPtr<UObject>>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_SOFTOBJECT_REF(TSoftObjectPtr<UObject>, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TSoftObjectPtr<UObject>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_SOFTOBJECT_REF(TSoftObjectPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TSoftObjectPtr<UObject>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_SOFTOBJECT_REF(TSoftObjectPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TSoftObjectPtr<UObject>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataTSoftObjectPtrLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(TSoftObjectPtr<UObject>, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<TSoftObjectPtr<UObject>>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataTSoftObjectPtrLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FSoftObjectPath& Value)
{
	Serializer->WriteValue(Value.ToString());
}

FSoftObjectPath UPsDataTSoftObjectPtrLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FSoftObjectPath& Value)
{
	FString String;
	if (Deserializer->ReadValue(String))
	{
		return FSoftObjectPath(String);
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *PsDataTools::FType<FSoftObjectPath>::Type());
	return {};
}
