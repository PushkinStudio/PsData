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

void UPsDataTSoftClassPtrLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const FSoftObjectPath& SoftObjectPath)
{
	static const FString AssetPathNameParam(TEXT("AssetPathName"));
	static const FString SubPathStringParam(TEXT("SubPathString"));

	Serializer->WriteObject();
	Serializer->WriteKey(AssetPathNameParam);
	Serializer->WriteValue(SoftObjectPath.GetAssetPathName());
	Serializer->PopKey(AssetPathNameParam);
	Serializer->WriteKey(SubPathStringParam);
	Serializer->WriteValue(SoftObjectPath.GetSubPathString());
	Serializer->PopKey(SubPathStringParam);
	Serializer->PopObject();
}

FSoftObjectPath UPsDataTSoftClassPtrLibrary::TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const FSoftObjectPath& SoftObjectPath)
{
	static const FString AssetPathNameParam(TEXT("AssetPathName"));
	static const FString SubPathStringParam(TEXT("SubPathString"));

	if (Deserializer->ReadObject())
	{
		FName AssetPathNameValue;
		FString SubPathStringValue;
		bool bHasAssetPathName = false;
		bool bHasSubPathString = false;

		FString Key;
		while (Deserializer->ReadKey(Key))
		{
			if (Key == AssetPathNameParam)
			{
				bHasAssetPathName = true;
				Deserializer->ReadValue(AssetPathNameValue);
			}
			else if (Key == SubPathStringParam)
			{
				bHasSubPathString = true;
				Deserializer->ReadValue(SubPathStringValue);
			}
			Deserializer->PopKey(Key);
		}
		Deserializer->PopObject();

		if (!bHasAssetPathName || !bHasSubPathString)
		{
			UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" (Object must have field: \"%s\")"), *Instance->GetClass()->GetName(), *Field->Name, *AssetPathNameParam);
		}

		return FSoftObjectPath(AssetPathNameValue, SubPathStringValue);
	}
	else
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\""), *Instance->GetClass()->GetName(), *Field->Name);
		return FSoftObjectPath{};
	}
}