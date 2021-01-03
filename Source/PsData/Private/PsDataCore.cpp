// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsDataCore.h"

#include "Types/PsData_FString.h"
#include "Types/PsData_UPsData.h"

#include "UObject/UObjectIterator.h"

namespace PsDataTools
{

TMap<UClass*, FClassFields> FDataReflection::FieldsByClass;
TMap<FString, const TArray<FString>> FDataReflection::SplittedPath;
TArray<const char*> FDataReflection::MetaCollection;
bool FDataReflection::bCompiled = false;

void FDataReflection::InitField(const char* CharName, int32 Hash, FAbstractDataTypeContext* Context, TSharedPtr<FDataField>& Field, UPsData* Instance, FAbstractDataProperty* Property)
{
	if (!bCompiled)
	{
		const FString FieldName(CharName);

		UClass* OwnerClass = Instance->GetClass();
		auto& Fields = FieldsByClass.FindOrAdd(OwnerClass);

#if !UE_BUILD_SHIPPING
		if (Fields.FieldsByHash.Contains(Hash))
		{
			UE_LOG(LogData, Fatal, TEXT("Can't generate unique hash for property %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
		}
		else if (Fields.FieldsByName.Contains(CharName))
		{
			UE_LOG(LogData, Fatal, TEXT("Duplicate name for property %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
		}
#endif

		const int32 Index = Fields.FieldsByName.Num();

		if (Field.IsValid())
		{
#if !UE_BUILD_SHIPPING
			check(FieldsByClass.FindChecked(OwnerClass->GetSuperClass()).FieldsByHash.FindChecked(Hash) == Field);
#endif
		}
		else
		{
			Field = MakeShared<FDataField>(FieldName, Index, Hash, Context, MetaCollection);
		}

		const FString& AliasName = Field->Meta.bAlias ? Field->Meta.Alias : FieldName;

#if !UE_BUILD_SHIPPING
		if (Fields.FieldsByAlias.Contains(AliasName) || Fields.FieldsByName.Contains(AliasName))
		{
			UE_LOG(LogData, Fatal, TEXT("Duplicate alias for property %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
		}
#endif

		MetaCollection.Reset();

		Fields.FieldsByName.Add(FieldName, Field);
		Fields.FieldsByAlias.Add(AliasName, Field);
		Fields.FieldsByHash.Add(Hash, Field);

		UE_LOG(LogData, VeryVerbose, TEXT(" %02d %s %s::%s (%d)"), Index + 1, *Context->GetCppType(), *OwnerClass->GetName(), *FieldName, Hash);
	}

	PsDataTools::FPsDataFriend::GetProperties(Instance).Add(Property);
}

void FDataReflection::InitLink(const char* CharName, const char* CharPath, const char* CharReturnType, int32 Hash, bool bAbstract, bool bCollection, UPsData* Instance)
{
	if (!bCompiled)
	{
		const FString Name(CharName);
		const FString Path(CharPath);
		const FString ReturnType(CharReturnType);

		UClass* OwnerClass = Instance->GetClass();

		bool bPathProperty = false;
		if (!bAbstract)
		{
			auto& PathField = GetFieldByName(OwnerClass, Path);
			if (PathField.IsValid())
			{
				if (PathField->Context->IsA(&PsDataTools::GetContext<FString>()))
				{
					bPathProperty = true;
				}
				else
				{
					UE_LOG(LogData, Fatal, TEXT("Can't describe link \"%s\" because path property \"%s\" has unsupported type \"%s\""), *Name, *Path, *PathField->Context->GetCppType());
				}
			}
		}

		auto& Fields = FieldsByClass.FindOrAdd(OwnerClass);

		if (const auto Find = Fields.LinksByHash.Find(Hash))
		{
			const auto Link = *Find;
			if (!Link->bAbstract)
			{
				UE_LOG(LogData, Fatal, TEXT("Can't override link for %s::%s %d"), *OwnerClass->GetName(), *Link->Name, Hash);
			}
		}

		const TSharedPtr<const FDataLink> Link = MakeShared<FDataLink>(Name, Path, bPathProperty, ReturnType, Hash, bAbstract, bCollection, MetaCollection);
		MetaCollection.Reset();

		Fields.LinksByName.Add(Name, Link);
		Fields.LinksByHash.Add(Hash, Link);
	}
}

void FDataReflection::InitMeta(const char* Meta)
{
	if (!bCompiled)
	{
		MetaCollection.Add(Meta);
	}
}

void FDataReflection::PreConstruct(UPsData* Instance)
{
	if (!bCompiled)
	{
		UClass* Class = Instance->GetClass();
		if (UPsData::StaticClass() == Class)
		{
			return;
		}

		if (FieldsByClass.Contains(Class))
		{
			return;
		}

		if ((Class->GetClassFlags() & CLASS_Constructed) == 0)
		{
			return;
		}

		UE_LOG(LogData, VeryVerbose, TEXT("Describe %s:"), *Class->GetName())
	}
}

void FDataReflection::PostConstruct(UPsData* Instance)
{
	if (!bCompiled)
	{
		UClass* Class = Instance->GetClass();
		if (MetaCollection.Num() > 0)
		{
			UE_LOG(LogData, Error, TEXT(" %s has unused meta"), *Class->GetName());
			MetaCollection.Reset();
		}

		auto& Fields = FieldsByClass.FindOrAdd(Class);
		Fields.FieldsByHash.KeySort([](const int32& a, const int32& b) -> bool {
			return a < b;
		});

		Fields.FieldsByName.KeySort([](const FString& a, const FString& b) -> bool {
			return a < b;
		});

		Fields.FieldsByAlias.KeySort([](const FString& a, const FString& b) -> bool {
			return a < b;
		});

		UE_LOG(LogData, VeryVerbose, TEXT("%s complete!"), *Class->GetName())
	}
}

void FDataReflection::Fill(UPsData* Instance)
{
	const bool bDefaultObject = Instance->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject | EObjectFlags::RF_DefaultSubObject);
	if (bDefaultObject)
	{
		return;
	}

	auto& Properties = PsDataTools::FPsDataFriend::GetProperties(Instance);
	for (const auto& Pair : FDataReflection::GetFields(Instance->GetClass()))
	{
		const auto& Field = Pair.Value;
		if (Field->Meta.bStrict)
		{
			Properties[Field->Index]->Allocate(Instance);
		}
	}

	Properties.Shrink();

	if (!bDefaultObject)
	{
		PsDataTools::FPsDataFriend::InitProperties(Instance);
	}
}

const TArray<FString>& FDataReflection::SplitPath(const FString& Path)
{
	if (const auto Find = SplittedPath.Find(Path))
	{
		return *Find;
	}

	TArray<FString> PathArray;
	Path.ParseIntoArray(PathArray, TEXT("."));
	return SplittedPath.Add(Path, std::move(PathArray));
}

const TSharedPtr<const FDataField>& FDataReflection::GetFieldByName(UClass* OwnerClass, const FString& Name)
{
	if (auto MapPtr = FieldsByClass.Find(OwnerClass))
	{
		if (const auto FieldPtr = MapPtr->FieldsByName.Find(Name))
		{
			return *FieldPtr;
		}
	}

	static const TSharedPtr<const FDataField> EmptySharedPtr(nullptr);
	return EmptySharedPtr;
}

const TSharedPtr<const FDataField>& FDataReflection::GetFieldByAlias(UClass* OwnerClass, const FString& Alias)
{
	if (auto MapPtr = FieldsByClass.Find(OwnerClass))
	{
		if (const auto FieldPtr = MapPtr->FieldsByAlias.Find(Alias))
		{
			return *FieldPtr;
		}
	}

	static const TSharedPtr<const FDataField> EmptySharedPtr(nullptr);
	return EmptySharedPtr;
}

const TSharedPtr<const FDataField>& FDataReflection::GetFieldByHash(UClass* OwnerClass, int32 Hash)
{
	if (auto MapPtr = FieldsByClass.Find(OwnerClass))
	{
		if (const auto FieldPtr = MapPtr->FieldsByHash.Find(Hash))
		{
			return *FieldPtr;
		}
	}

	static const TSharedPtr<const FDataField> EmptySharedPtr(nullptr);
	return EmptySharedPtr;
}

const TMap<FString, const TSharedPtr<const FDataField>>& FDataReflection::GetFields(const UClass* OwnerClass)
{
	const auto Find = FieldsByClass.Find(OwnerClass);
	if (Find)
	{
		return Find->FieldsByName;
	}

	static TMap<FString, const TSharedPtr<const FDataField>> Empty;
	return Empty;
}

const TMap<FString, const TSharedPtr<const FDataField>>& FDataReflection::GetAliasFields(const UClass* OwnerClass)
{
	const auto Find = FieldsByClass.Find(OwnerClass);
	if (Find)
	{
		return Find->FieldsByAlias;
	}

	static TMap<FString, const TSharedPtr<const FDataField>> Empty;
	return Empty;
}

const TSharedPtr<const FDataLink>& FDataReflection::GetLinkByName(UClass* OwnerClass, const FString& Name)
{
	if (auto MapPtr = FieldsByClass.Find(OwnerClass))
	{
		if (const auto FieldPtr = MapPtr->LinksByName.Find(Name))
		{
			return *FieldPtr;
		}
	}

	static const TSharedPtr<const FDataLink> EmptySharedPtr(nullptr);
	return EmptySharedPtr;
}

const TSharedPtr<const FDataLink>& FDataReflection::GetLinkByHash(UClass* OwnerClass, int32 Hash)
{
	if (auto MapPtr = FieldsByClass.Find(OwnerClass))
	{
		if (const auto FieldPtr = MapPtr->LinksByHash.Find(Hash))
		{
			return *FieldPtr;
		}
	}

	static const TSharedPtr<const FDataLink> EmptySharedPtr(nullptr);
	return EmptySharedPtr;
}

const TMap<FString, const TSharedPtr<const FDataLink>>& FDataReflection::GetLinks(UClass* OwnerClass)
{
	const auto Find = FieldsByClass.Find(OwnerClass);
	if (Find)
	{
		return Find->LinksByName;
	}

	static TMap<FString, const TSharedPtr<const FDataLink>> Empty;
	return Empty;
}

bool FDataReflection::HasClass(const UClass* OwnerClass)
{
	return FieldsByClass.Contains(OwnerClass);
}

void FDataReflection::Compile()
{
	check(!bCompiled);
	bCompiled = true;

	MetaCollection.Shrink();

	TArray<UField*> ReadOnlyFields;
	for (auto& MapPair : FieldsByClass)
	{
		for (auto& Pair : MapPair.Value.FieldsByHash)
		{
			if (Pair.Value->Meta.bReadOnly)
			{
				ReadOnlyFields.Add(Pair.Value->Context->GetUE4Type());
			}
		}
	}

	while (ReadOnlyFields.Num() > 0)
	{
		UField* UEField = ReadOnlyFields.Pop();
		if (UClass* Class = Cast<UClass>(UEField))
		{
			if (auto Find = FieldsByClass.Find(Class))
			{
				for (auto& Pair : Find->FieldsByHash)
				{
					if (!Pair.Value->Meta.bReadOnly)
					{
						FDataField* Field = const_cast<FDataField*>(Pair.Value.Get());
						Field->Meta.bReadOnly = true;
						if (Pair.Value->Context->IsData())
						{
							ReadOnlyFields.AddUnique(Pair.Value->Context->GetUE4Type());
						}
					}
				}
			}
		}
	}
}

} // namespace PsDataTools