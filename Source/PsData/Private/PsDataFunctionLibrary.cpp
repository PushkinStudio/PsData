// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsDataFunctionLibrary.h"

#include "Collection/PsDataBlueprintArrayProxy.h"
#include "Collection/PsDataBlueprintMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataRoot.h"
#include "Types/PsData_FName.h"
#include "Types/PsData_FString.h"
#include "Types/PsData_UPsData.h"
#include "Types/PsData_uint8.h"

/***********************************
 * Link
 ***********************************/

const FString& UPsDataFunctionLibrary::GetLinkPath(const UPsData* ConstTarget, TSharedPtr<const FDataLink> Link)
{
	//TODO: PS-136
	UPsData* Target = const_cast<UPsData*>(ConstTarget);
	if (Link->bPathProperty)
	{
		FString* PropertyPtr = nullptr;
		if (PsDataTools::GetByName<FString>(Target, Link->Path, PropertyPtr))
		{
			return *PropertyPtr;
		}
		else
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property with path \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName())
		}
	}
	return Link->Path;
}

void UPsDataFunctionLibrary::GetLinkKeys(const UPsData* ConstTarget, TSharedPtr<const FDataLink> Link, TArray<FString>& OutKeys)
{
	//TODO: PS-136
	UPsData* Target = const_cast<UPsData*>(ConstTarget);
	auto& Field = PsDataTools::FDataReflection::GetFieldByName(Target->GetClass(), Link->Name);
	check(Field.IsValid());

	if (Field->Context->IsA(&PsDataTools::GetContext<FString>()))
	{
		FString* PropertyPtr = nullptr;
		if (PsDataTools::GetByField<FString>(Target, Field, PropertyPtr))
		{
			OutKeys.Add(*PropertyPtr);
			return;
		}
	}
	else if (Field->Context->IsA(&PsDataTools::GetContext<TArray<FString>>()))
	{
		TArray<FString>* PropertyPtr = nullptr;
		if (PsDataTools::GetByField<TArray<FString>>(Target, Field, PropertyPtr))
		{
			OutKeys.Append(*PropertyPtr);
			return;
		}
	}
	else if (Field->Context->IsA(&PsDataTools::GetContext<FName>()))
	{
		FName* PropertyPtr = nullptr;
		if (PsDataTools::GetByField<FName>(Target, Field, PropertyPtr))
		{
			if (*PropertyPtr == NAME_None)
			{
				OutKeys.Add(TEXT(""));
			}
			else
			{
				OutKeys.Add(PropertyPtr->ToString().ToLower());
			}
			return;
		}
	}
	else if (Field->Context->IsA(&PsDataTools::GetContext<TArray<FName>>()))
	{
		TArray<FName>* PropertyPtr = nullptr;
		if (PsDataTools::GetByField<TArray<FName>>(Target, Field, PropertyPtr))
		{
			OutKeys.Reserve(PropertyPtr->Num());
			for (const FName& Name : *PropertyPtr)
			{
				if (Name != NAME_None)
				{
					OutKeys.Add(Name.ToString().ToLower());
				}
			}
			return;
		}
	}
	else if (Field->Context->IsEnum())
	{
		UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());
		if (!Enum)
		{
			UE_LOG(LogData, Fatal, TEXT("Unsupported enum \"%s\""), *Field->Context->GetCppType());
		}

		if (!Field->Context->IsContainer())
		{
			uint8* PropertyPtr = nullptr;
			if (PsDataTools::GetByField<uint8>(Target, Field, PropertyPtr))
			{
				OutKeys.Add(Enum->GetNameStringByValue(static_cast<int64>(*PropertyPtr)).ToLower());
				return;
			}
		}
		else if (Field->Context->IsArray())
		{
			TArray<uint8>* PropertyPtr = nullptr;
			if (PsDataTools::GetByField<TArray<uint8>>(Target, Field, PropertyPtr))
			{
				OutKeys.Reserve(PropertyPtr->Num());
				for (const uint8& Byte : *PropertyPtr)
				{
					OutKeys.Add(Enum->GetNameStringByValue(static_cast<int64>(Byte)).ToLower());
				}
				return;
			}
		}
		else
		{
			UE_LOG(LogData, Fatal, TEXT("Unsupported type \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName());
		}
	}
	else
	{
		UE_LOG(LogData, Fatal, TEXT("Unsupported type \"%s\" in \"%s\""), *Link->Name, *Target->GetClass()->GetName());
	}

	UE_LOG(LogData, Fatal, TEXT("Can't find property \"%s\" in \"%s\""), *Field->Name, *Target->GetClass()->GetName());
}

UPsData* UPsDataFunctionLibrary::GetDataByLinkHash(const UPsData* ConstTarget, int32 Hash)
{
	//TODO: PS-136
	UPsData* Target = const_cast<UPsData*>(ConstTarget);
	const TSharedPtr<const FDataLink> Link = PsDataTools::FDataReflection::GetLinkByHash(Target->GetClass(), Hash);
	check(Link.IsValid());
	check(!Link->bAbstract);

	TArray<FString> Keys;
	GetLinkKeys(ConstTarget, Link, Keys);

	const auto RootData = Target->GetRoot();
	check(RootData);

	const FString& LinkPath = GetLinkPath(ConstTarget, Link);
	TMap<FString, UPsData*>* MapPtr = nullptr;
	if (!PsDataTools::GetByPath<TMap<FString, UPsData*>>(RootData, LinkPath, MapPtr))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find path \"%s\" in \"%s\""), *LinkPath, *Target->GetClass()->GetName())
	}

	UPsData** Find = MapPtr->Find(Keys[0]);
	if (Find)
	{
		return *Find;
	}

	if (!Link->Meta.bNullable)
	{
		UE_LOG(LogData, Fatal, TEXT("Link without Nullable meta can't be nullptr"))
	}

	return nullptr;
}

TArray<UPsData*> UPsDataFunctionLibrary::GetDataArrayByLinkHash(const UPsData* ConstTarget, int32 Hash)
{
	//TODO: PS-136
	UPsData* Target = const_cast<UPsData*>(ConstTarget);
	const TSharedPtr<const FDataLink> Link = PsDataTools::FDataReflection::GetLinkByHash(Target->GetClass(), Hash);
	check(Link.IsValid());
	check(!Link->bAbstract);

	TArray<FString> Keys;
	GetLinkKeys(ConstTarget, Link, Keys);

	const auto RootData = Target->GetRoot();
	check(RootData);

	const FString& LinkPath = GetLinkPath(ConstTarget, Link);
	TMap<FString, UPsData*>* MapPtr = nullptr;
	if (!PsDataTools::GetByPath<TMap<FString, UPsData*>>(RootData, LinkPath, MapPtr))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't find path \"%s\" in \"%s\""), *LinkPath, *Target->GetClass()->GetName())
	}

	TArray<UPsData*> Result;
	for (const FString& Key : Keys)
	{
		UPsData** Find = MapPtr->Find(Key);
		if (Find)
		{
			Result.Add(*Find);
		}
		else if (Link->Meta.bNullable)
		{
			Result.Add(nullptr);
		}
		else
		{
			UE_LOG(LogData, Fatal, TEXT("Link without Nullable meta can't be nullptr"))
		}
	}
	return Result;
}

bool UPsDataFunctionLibrary::IsLinkEmpty(const UPsData* ConstTarget, int32 Hash)
{
	const TSharedPtr<const FDataLink> Link = PsDataTools::FDataReflection::GetLinkByHash(ConstTarget->GetClass(), Hash);
	if (Link.IsValid())
	{
		if (!Link->Meta.bNullable)
		{
			return false;
		}

		TArray<FString> Keys;
		GetLinkKeys(ConstTarget, Link, Keys);
		for (const FString& Key : Keys)
		{
			if (Key.Len() > 0)
			{
				return false;
			}
		}
	}
	return true;
}

/***********************************
 * Blueprint collections proxy
 ***********************************/

UPsDataBlueprintMapProxy* UPsDataFunctionLibrary::GetMapProxy(UPsData* Target, int32 Hash)
{
	//TODO: Always NewObject?
	UPsDataBlueprintMapProxy* Result = NewObject<UPsDataBlueprintMapProxy>();
	Result->Init(Target, PsDataTools::FDataReflection::GetFieldByIndex(Target->GetClass(), Hash));
	return Result;
}

UPsDataBlueprintArrayProxy* UPsDataFunctionLibrary::GetArrayProxy(UPsData* Target, int32 Hash)
{
	//TODO: Always NewObject?
	UPsDataBlueprintArrayProxy* Result = NewObject<UPsDataBlueprintArrayProxy>();
	Result->Init(Target, PsDataTools::FDataReflection::GetFieldByIndex(Target->GetClass(), Hash));
	return Result;
}
