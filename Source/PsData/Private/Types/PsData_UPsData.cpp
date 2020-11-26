// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Types/PsData_UPsData.h"

#include "Serialize/PsDataSerialization.h"

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, UPsData*, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TMap<FString, UPsData*>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TMAP_REF(FString, UPsData*, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, UPsData*>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(UPsData*, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<TArray<UPsData*>>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_TARRAY_REF(UPsData*, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<UPsData*>* Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_OBJECT(UPsData, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::SetByHash<UPsData*>(Target, Hash, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Hash);
	P_GET_OBJECT_REF(UPsData, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	UPsData** Result = nullptr;
	PsDataTools::GetByHash(Target, Hash, Result);
	Out = *Result;
	P_NATIVE_END;
}

void UPsDataUPsDataLibrary::TypeSerialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const void* Value)
{
	Serializer->WriteValue(static_cast<const UPsData*>(Value));
}

void* UPsDataUPsDataLibrary::TypeDeserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, void* Value)
{
	FPsDataAllocator Allocator(CastChecked<UClass>(Field->Context->GetUE4Type()), Instance);

	UPsData* NewValue = static_cast<UPsData*>(Value);
	if (!Deserializer->ReadValue(NewValue, Allocator))
	{
		UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\""), *Instance->GetClass()->GetName(), *Field->Name);
	}

	return NewValue;
}

bool UPsDataUPsDataLibrary::IsA(const FAbstractDataTypeContext* LeftContext, const FAbstractDataTypeContext* RightContext)
{
	UClass* RClass = Cast<UClass>(RightContext->GetUE4Type());
	//if (RClass != nullptr && RClass->IsChildOf(T::StaticClass()))
	if (RClass != nullptr && RClass->IsChildOf(UPsData::StaticClass()))
		return true;
	return false;
}