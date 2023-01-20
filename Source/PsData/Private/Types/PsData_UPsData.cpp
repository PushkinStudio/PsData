// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Types/PsData_UPsData.h"

#include "PsDataDefines.h"
#include "PsDataLink.h"
#include "Serialize/PsDataSerialization.h"

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execSetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, UPsData*, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TMap<FString, UPsData*>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetMapProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TMAP_REF(FString, UPsData*, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TMap<FString, UPsData*>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execSetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(UPsData*, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<TArray<UPsData*>>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetArrayProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(UPsData*, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<UPsData*>* Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execSetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_OBJECT(UPsData, Value);
	P_FINISH;
	P_NATIVE_BEGIN;
	PsDataTools::UnsafeSetByIndex<UPsData*>(Target, Index, Value);
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetProperty)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_OBJECT_REF(UPsData, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	UPsData** Result = nullptr;
	PsDataTools::UnsafeGetByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_OBJECT_REF(UPsData, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	UPsData** Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

DEFINE_FUNCTION(UPsDataUPsDataLibrary::execGetArrayLinkValue)
{
	P_GET_OBJECT(UPsData, Target);
	P_GET_PROPERTY(FIntProperty, Index);
	P_GET_TARRAY_REF(UPsData*, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	TArray<UPsData*>* Result = nullptr;
	PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
	Out = *Result;
	P_NATIVE_END;
}

FString SerializeClass(const UClass* Class)
{
	return Class->GetName();
}

UClass* DeserializeClass(const FString& ClassName)
{
	return FindObject<UClass>(ANY_PACKAGE, *ClassName);
}

static const FString CustomTypeParam(TEXT("CustomType"));

void UPsDataUPsDataLibrary::TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const void* Value)
{
	const auto ValueData = static_cast<const UPsData*>(Value);
	if (Field->Meta.bCustomType)
	{
		Serializer->WriteObject();
		Serializer->WriteKey(CustomTypeParam);
		Serializer->WriteArray();
		Serializer->WriteValue(SerializeClass(ValueData ? ValueData->GetClass() : CastChecked<UClass>(Field->Context->GetUEType())));
		Serializer->WriteValue(ValueData);
		Serializer->PopArray();
		Serializer->PopKey(CustomTypeParam);
		Serializer->PopObject();
	}
	else
	{
		Serializer->WriteValue(ValueData);
	}
}

void* UPsDataUPsDataLibrary::TypeDeserialize(UPsData* Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, void* Value)
{
	UPsData* CurrentValue = static_cast<UPsData*>(Value);

	bool bDeserialized = false;
	if (Field->Meta.bCustomType)
	{
		if (Deserializer->ReadObject())
		{
			FString CustomTypeKey;
			if (Deserializer->ReadKey(CustomTypeKey))
			{
				if (CustomTypeKey == CustomTypeParam)
				{
					if (Deserializer->ReadArray())
					{
						if (Deserializer->ReadIndex())
						{
							FString TypeValue;
							const bool bTypeContains = Deserializer->ReadValue(TypeValue);
							Deserializer->PopIndex();

							auto Class = DeserializeClass(TypeValue);
							if (!Class)
							{
								Class = CastChecked<UClass>(Field->Context->GetUEType());
								UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\" as Custom Type: %s"), *Instance->GetClass()->GetName(), *Field->Name, *TypeValue);
							}

							if (CurrentValue && CurrentValue->GetClass() != Class)
							{
								CurrentValue = nullptr;
							}

							if (bTypeContains && Deserializer->ReadIndex())
							{
								if (Deserializer->ReadValue(CurrentValue, FPsDataAllocator(Class, Instance)))
								{
									bDeserialized = true;
								}
								Deserializer->PopIndex();
							}
						}
						Deserializer->PopArray();
					}
				}
				Deserializer->PopKey(CustomTypeKey);
			}
			Deserializer->PopObject();
		}
	}
	else
	{
		if (Deserializer->ReadValue(CurrentValue, FPsDataAllocator(CastChecked<UClass>(Field->Context->GetUEType()), Instance)))
		{
			bDeserialized = true;
		}
	}

	if (bDeserialized)
	{
		return CurrentValue;
	}

	UE_LOG(LogData, Warning, TEXT("Can't deserialize \"%s::%s\""), *Instance->GetClass()->GetName(), *Field->Name);
	return nullptr;
}

bool UPsDataUPsDataLibrary::IsA(const FAbstractDataTypeContext* LeftContext, const FAbstractDataTypeContext* RightContext)
{
	UClass* RClass = Cast<UClass>(RightContext->GetUEType());
	// if (RClass != nullptr && RClass->IsChildOf(T::StaticClass()))
	if (RClass != nullptr && RClass->IsChildOf(UPsData::StaticClass()))
		return true;
	return false;
}
