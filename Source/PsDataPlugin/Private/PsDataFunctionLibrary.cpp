// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataFunctionLibrary.h"

#include "Collection/PsDataBlueprintArrayProxy.h"
#include "Collection/PsDataBlueprintMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"

/***********************************
 * int32
 ***********************************/

int32 UPsDataFunctionLibrary::GetIntProperty(UPsData* Target, int32 Hash)
{
	int32* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	return 0;
}

void UPsDataFunctionLibrary::SetIntProperty(UPsData* Target, int32 Hash, int32 Value)
{
	FDataReflectionTools::SetByHash<int32>(Target, Hash, Value);
}

const TArray<int32>& UPsDataFunctionLibrary::GetIntArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<int32>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<int32> Default = TArray<int32>();
	return Default;
}

void UPsDataFunctionLibrary::SetIntArrayProperty(UPsData* Target, int32 Hash, const TArray<int32>& Value)
{
	FDataReflectionTools::SetByHash<TArray<int32>>(Target, Hash, Value);
}

const TMap<FString, int32>& UPsDataFunctionLibrary::GetIntMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, int32>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, int32> Default = TMap<FString, int32>();
	return Default;
}

void UPsDataFunctionLibrary::SetIntMapProperty(UPsData* Target, int32 Hash, const TMap<FString, int32>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, int32>>(Target, Hash, Value);
}

/***********************************
 * uint8
 ***********************************/

uint8 UPsDataFunctionLibrary::GetByteProperty(UPsData* Target, int32 Hash)
{
	uint8* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	return 0;
}

void UPsDataFunctionLibrary::SetByteProperty(UPsData* Target, int32 Hash, uint8 Value)
{
	FDataReflectionTools::SetByHash<uint8>(Target, Hash, Value);
}

const TArray<uint8>& UPsDataFunctionLibrary::GetByteArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<uint8>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<uint8> Default = TArray<uint8>();
	return Default;
}

void UPsDataFunctionLibrary::SetByteArrayProperty(UPsData* Target, int32 Hash, const TArray<uint8>& Value)
{
	FDataReflectionTools::SetByHash<TArray<uint8>>(Target, Hash, Value);
}

const TMap<FString, uint8>& UPsDataFunctionLibrary::GetByteMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, uint8>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, uint8> Default = TMap<FString, uint8>();
	return Default;
}

void UPsDataFunctionLibrary::SetByteMapProperty(UPsData* Target, int32 Hash, const TMap<FString, uint8>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, uint8>>(Target, Hash, Value);
}

/***********************************
 * float
 ***********************************/

float UPsDataFunctionLibrary::GetFloatProperty(UPsData* Target, int32 Hash)
{
	float* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	return 0.f;
}

void UPsDataFunctionLibrary::SetFloatProperty(UPsData* Target, int32 Hash, float Value)
{
	FDataReflectionTools::SetByHash<float>(Target, Hash, Value);
}

const TArray<float>& UPsDataFunctionLibrary::GetFloatArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<float>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<float> Default = TArray<float>();
	return Default;
}

void UPsDataFunctionLibrary::SetFloatArrayProperty(UPsData* Target, int32 Hash, const TArray<float>& Value)
{
	FDataReflectionTools::SetByHash<TArray<float>>(Target, Hash, Value);
}

const TMap<FString, float>& UPsDataFunctionLibrary::GetFloatMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, float>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, float> Default = TMap<FString, float>();
	return Default;
}

void UPsDataFunctionLibrary::SetFloatMapProperty(UPsData* Target, int32 Hash, const TMap<FString, float>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, float>>(Target, Hash, Value);
}

/***********************************
 * bool
 ***********************************/

bool UPsDataFunctionLibrary::GetBoolProperty(UPsData* Target, int32 Hash)
{
	bool* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	return false;
}

void UPsDataFunctionLibrary::SetBoolProperty(UPsData* Target, int32 Hash, bool Value)
{
	FDataReflectionTools::SetByHash<bool>(Target, Hash, Value);
}

const TArray<bool>& UPsDataFunctionLibrary::GetBoolArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<bool>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<bool> Default = TArray<bool>();
	return Default;
}

void UPsDataFunctionLibrary::SetBoolArrayProperty(UPsData* Target, int32 Hash, const TArray<bool>& Value)
{
	FDataReflectionTools::SetByHash<TArray<bool>>(Target, Hash, Value);
}

const TMap<FString, bool>& UPsDataFunctionLibrary::GetBoolMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, bool>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, bool> Default = TMap<FString, bool>();
	return Default;
}

void UPsDataFunctionLibrary::SetBoolMapProperty(UPsData* Target, int32 Hash, const TMap<FString, bool>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, bool>>(Target, Hash, Value);
}

/***********************************
 * String
 ***********************************/

FString UPsDataFunctionLibrary::GetStringProperty(UPsData* Target, int32 Hash)
{
	FString* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	return TEXT("");
}

void UPsDataFunctionLibrary::SetStringProperty(UPsData* Target, int32 Hash, const FString& Value)
{
	FDataReflectionTools::SetByHash<FString>(Target, Hash, Value);
}

const TArray<FString>& UPsDataFunctionLibrary::GetStringArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<FString>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<FString> Default = TArray<FString>();
	return Default;
}

void UPsDataFunctionLibrary::SetStringArrayProperty(UPsData* Target, int32 Hash, const TArray<FString>& Value)
{
	FDataReflectionTools::SetByHash<TArray<FString>>(Target, Hash, Value);
}

const TMap<FString, FString>& UPsDataFunctionLibrary::GetStringMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, FString>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, FString> Default = TMap<FString, FString>();
	return Default;
}

void UPsDataFunctionLibrary::SetStringMapProperty(UPsData* Target, int32 Hash, const TMap<FString, FString>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, FString>>(Target, Hash, Value);
}

/***********************************
 * FText
 ***********************************/

FText UPsDataFunctionLibrary::GetTextProperty(UPsData* Target, int32 Hash)
{
	FText* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const FText Default = FText();
	return Default;
}

void UPsDataFunctionLibrary::SetTextProperty(UPsData* Target, int32 Hash, const FText& Value)
{
	FDataReflectionTools::SetByHash<FText>(Target, Hash, Value);
}

const TArray<FText>& UPsDataFunctionLibrary::GetTextArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<FText>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<FText> Default = TArray<FText>();
	return Default;
}

void UPsDataFunctionLibrary::SetTextArrayProperty(UPsData* Target, int32 Hash, const TArray<FText>& Value)
{
	FDataReflectionTools::SetByHash<TArray<FText>>(Target, Hash, Value);
}

const TMap<FString, FText>& UPsDataFunctionLibrary::GetTextMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, FText>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, FText> Default = TMap<FString, FText>();
	return Default;
}

void UPsDataFunctionLibrary::SetTextMapProperty(UPsData* Target, int32 Hash, const TMap<FString, FText>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, FText>>(Target, Hash, Value);
}

/***********************************
 * Data
 ***********************************/

UPsData* UPsDataFunctionLibrary::GetDataProperty(UPsData* Target, int32 Hash)
{
	UPsData** Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	return nullptr;
}

void UPsDataFunctionLibrary::SetDataProperty(UPsData* Target, int32 Hash, UPsData* Value)
{
	FDataReflectionTools::SetByHash<UPsData*>(Target, Hash, Value);
}

const TArray<UPsData*>& UPsDataFunctionLibrary::GetDataArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<UPsData*>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<UPsData*> Default = TArray<UPsData*>();
	return Default;
}

void UPsDataFunctionLibrary::SetDataArrayProperty(UPsData* Target, int32 Hash, const TArray<UPsData*>& Value)
{
	FDataReflectionTools::SetByHash<TArray<UPsData*>>(Target, Hash, Value);
}

const TMap<FString, UPsData*>& UPsDataFunctionLibrary::GetDataMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, UPsData*>* Result = nullptr;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, UPsData*> Default = TMap<FString, UPsData*>();
	return Default;
}

void UPsDataFunctionLibrary::SetDataMapProperty(UPsData* Target, int32 Hash, const TMap<FString, UPsData*>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, UPsData*>>(Target, Hash, Value);
}

/***********************************
 * TSoftObject
 ***********************************/

const TSoftObjectPtr<UObject>& UPsDataFunctionLibrary::GetSoftObjectProperty(UPsData* Target, int32 Hash)
{
	TSoftObjectPtr<UObject>* Result;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TSoftObjectPtr<UObject> Default = TSoftObjectPtr<UObject>();
	return Default;
}

void UPsDataFunctionLibrary::SetSoftObjectProperty(UPsData* Target, int32 Hash, const TSoftObjectPtr<UObject>& Value)
{
	FDataReflectionTools::SetByHash<TSoftObjectPtr<UObject>>(Target, Hash, Value);
}

const TArray<TSoftObjectPtr<UObject>>& UPsDataFunctionLibrary::GetSoftObjectArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<TSoftObjectPtr<UObject>>* Result;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<TSoftObjectPtr<UObject>> Default = TArray<TSoftObjectPtr<UObject>>();
	return Default;
}

void UPsDataFunctionLibrary::SetSoftObjectArrayProperty(UPsData* Target, int32 Hash, const TArray<TSoftObjectPtr<UObject>>& Value)
{
	FDataReflectionTools::SetByHash<TArray<TSoftObjectPtr<UObject>>>(Target, Hash, Value);
}

const TMap<FString, TSoftObjectPtr<UObject>>& UPsDataFunctionLibrary::GetSoftObjectMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, TSoftObjectPtr<UObject>>* Result;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, TSoftObjectPtr<UObject>> Default = TMap<FString, TSoftObjectPtr<UObject>>();
	return Default;
}

void UPsDataFunctionLibrary::SetSoftObjectMapProperty(UPsData* Target, int32 Hash, const TMap<FString, TSoftObjectPtr<UObject>>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, TSoftObjectPtr<UObject>>>(Target, Hash, Value);
}

/***********************************
 * TSoftClass
 ***********************************/

const TSoftClassPtr<UObject>& UPsDataFunctionLibrary::GetSoftClassProperty(UPsData* Target, int32 Hash)
{
	TSoftClassPtr<UObject>* Result;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TSoftClassPtr<UObject> Default = TSoftClassPtr<UObject>();
	return Default;
}

void UPsDataFunctionLibrary::SetSoftClassProperty(UPsData* Target, int32 Hash, const TSoftClassPtr<UObject>& Value)
{
	FDataReflectionTools::SetByHash<TSoftClassPtr<UObject>>(Target, Hash, Value);
}

const TArray<TSoftClassPtr<UObject>>& UPsDataFunctionLibrary::GetSoftClassArrayProperty(UPsData* Target, int32 Hash)
{
	TArray<TSoftClassPtr<UObject>>* Result;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TArray<TSoftClassPtr<UObject>> Default = TArray<TSoftClassPtr<UObject>>();
	return Default;
}

void UPsDataFunctionLibrary::SetSoftClassArrayProperty(UPsData* Target, int32 Hash, const TArray<TSoftClassPtr<UObject>>& Value)
{
	FDataReflectionTools::SetByHash<TArray<TSoftClassPtr<UObject>>>(Target, Hash, Value);
}

const TMap<FString, TSoftClassPtr<UObject>>& UPsDataFunctionLibrary::GetSoftClassMapProperty(UPsData* Target, int32 Hash)
{
	TMap<FString, TSoftClassPtr<UObject>>* Result;
	if (FDataReflectionTools::GetByHash(Target, Hash, Result))
	{
		return *Result;
	}
	static const TMap<FString, TSoftClassPtr<UObject>> Default = TMap<FString, TSoftClassPtr<UObject>>();
	return Default;
}

void UPsDataFunctionLibrary::SetSoftClassMapProperty(UPsData* Target, int32 Hash, const TMap<FString, TSoftClassPtr<UObject>>& Value)
{
	FDataReflectionTools::SetByHash<TMap<FString, TSoftClassPtr<UObject>>>(Target, Hash, Value);
}

/***********************************
 * Path
 ***********************************/

UPsData* UPsDataFunctionLibrary::GetDataByLinkHash(const UPsData* ConstTarget, int32 Hash)
{
	//TODO: PS-136
	UPsData* Target = const_cast<UPsData*>(ConstTarget);

	TSharedPtr<const FDataLink> Link = FDataReflection::GetLinkByHash(Target->GetClass(), Hash);
	if (!Link.IsValid())
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find link 0x%08x in \"%s\""), Hash, *Target->GetClass()->GetName())
	}

	if (Link->bAbstract)
	{
		UE_LOG(LogData, Fatal, TEXT("Can't link abstract path \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName())
	}

	FString* PropertyPtr = nullptr;
	if (!FDataReflectionTools::GetByName<FString>(Target, Link->Name, PropertyPtr))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find property \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName())
	}

	TMap<FString, UPsData*>* MapPtr = nullptr;
	if (!FDataReflectionTools::GetByName<TMap<FString, UPsData*>>(Target->GetRoot(), Link->Path, MapPtr))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find path \"%s\" in \"%s\""), *Link->Path, *Target->GetRoot()->GetClass()->GetName())
	}

	UPsData** Find = MapPtr->Find(*PropertyPtr);
	if (Find)
	{
		return *Find;
	}

	return nullptr;
}

TArray<UPsData*> UPsDataFunctionLibrary::GetDataArrayByLinkHash(const UPsData* ConstTarget, int32 Hash)
{
	//TODO: PS-136
	UPsData* Target = const_cast<UPsData*>(ConstTarget);

	TSharedPtr<const FDataLink> Link = FDataReflection::GetLinkByHash(Target->GetClass(), Hash);
	if (!Link.IsValid())
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find link 0x%08x in \"%s\""), Hash, *Target->GetClass()->GetName())
	}

	if (Link->bAbstract)
	{
		UE_LOG(LogData, Fatal, TEXT("Can't link abstract path \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName())
	}

	TArray<FString>* PropertyPtr = nullptr;
	if (!FDataReflectionTools::GetByName<TArray<FString>>(Target, Link->Name, PropertyPtr))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find property \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName())
	}

	TMap<FString, UPsData*>* MapPtr = nullptr;
	if (!FDataReflectionTools::GetByName<TMap<FString, UPsData*>>(Target->GetRoot(), Link->Path, MapPtr))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find path \"%s\" in \"%s\""), *Link->Path, *Target->GetRoot()->GetClass()->GetName())
	}

	TArray<UPsData*> Result;
	const TArray<FString>& Array = *PropertyPtr;
	for (const FString& Element : Array)
	{
		UPsData** Find = MapPtr->Find(Element);
		if (Find)
		{
			Result.Add(*Find);
		}
	}
	return Result;
}

/***********************************
 * Blueprint collections proxy
 ***********************************/

UPsDataBlueprintMapProxy* UPsDataFunctionLibrary::GetMapProxy(UPsData* Target, int32 Crc32)
{
	//TODO: Always NewObject?
	UPsDataBlueprintMapProxy* Result = NewObject<UPsDataBlueprintMapProxy>();
	Result->Init(Target, FDataReflection::GetFieldByHash(Target->GetClass(), Crc32));
	return Result;
}

UPsDataBlueprintArrayProxy* UPsDataFunctionLibrary::GetArrayProxy(UPsData* Target, int32 Crc32)
{
	//TODO: Always NewObject?
	UPsDataBlueprintArrayProxy* Result = NewObject<UPsDataBlueprintArrayProxy>();
	Result->Init(Target, FDataReflection::GetFieldByHash(Target->GetClass(), Crc32));
	return Result;
}
