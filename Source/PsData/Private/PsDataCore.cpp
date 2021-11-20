// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsDataCore.h"

#include "Types/PsData_FString.h"
#include "Types/PsData_UPsData.h"

#include "UObject/UObjectIterator.h"

namespace PsDataTools
{
FClassFields::FClassFields()
{
}

FClassFields::~FClassFields()
{
	for (int32 i = 0; i < FieldsList.Num(); ++i)
	{
		auto Field = FieldsList[i];
		delete Field;
	}

	FieldsList.Empty();
	ConstFieldsList.Empty();

	for (int32 i = 0; i < LinkList.Num(); ++i)
	{
		auto Link = LinkList[i];
		delete Link;
	}

	LinkList.Empty();
	ConstLinkList.Empty();

	FieldsByName.Empty();
	FieldsByAlias.Empty();
	FieldsByHash.Empty();

	LinksByName.Empty();
	LinksByHash.Empty();
}

void FClassFields::AddField(FDataField* Field)
{
	FieldsList.Add(Field);
	ConstFieldsList.Add(Field);

	FieldsByName.Add(Field->Name, Field);
	FieldsByAlias.Add(Field->GetAliasName(), Field);
	FieldsByHash.Add(Field->Hash, Field);
}

void FClassFields::AddLink(FDataLink* Link)
{
	LinkList.RemoveAll([Link](const FDataLink* Item) {
		return Item->Hash == Link->Hash;
	});

	ConstLinkList.RemoveAll([Link](const FDataLink* Item) {
		return Item->Hash == Link->Hash;
	});

	LinkList.Add(Link);
	ConstLinkList.Add(Link);

	LinksByName.Add(Link->Name, Link);
	LinksByHash.Add(Link->Hash, Link);
}

void FClassFields::Sort()
{
	FieldsList.StableSort([](const FDataField& A, const FDataField& B) {
		return A.GetNameForSerialize() < B.GetNameForSerialize();
	});

	ConstFieldsList.StableSort([](const FDataField& A, const FDataField& B) {
		return A.GetNameForSerialize() < B.GetNameForSerialize();
	});

	FieldsByHash.ValueStableSort([](const FDataField& A, const FDataField& B) -> bool {
		return A.GetNameForSerialize() < B.GetNameForSerialize();
	});

	FieldsByName.ValueStableSort([](const FDataField& A, const FDataField& B) -> bool {
		return A.GetNameForSerialize() < B.GetNameForSerialize();
	});

	FieldsByAlias.ValueStableSort([](const FDataField& A, const FDataField& B) -> bool {
		return A.GetNameForSerialize() < B.GetNameForSerialize();
	});
}

const TArray<FDataField*>& FClassFields::GetFieldsList()
{
	return FieldsList;
}

const TArray<const FDataField*>& FClassFields::GetFieldsList() const
{
	return ConstFieldsList;
}

FDataField* FClassFields::GetMutableField(const FDataField* Field)
{
	for (FDataField* MutableField : FieldsList)
	{
		if (MutableField == Field)
		{
			return MutableField;
		}
	}

	checkNoEntry();
	return nullptr;
}

const TArray<FDataLink*>& FClassFields::GetLinksList()
{
	return LinkList;
}

const TArray<const FDataLink*>& FClassFields::GetLinksList() const
{
	return ConstLinkList;
}

const FDataField* FClassFields::GetFieldByHash(int32 Hash) const
{
	const auto FieldPtr = FieldsByHash.Find(Hash);
	return FieldPtr ? *FieldPtr : nullptr;
}

const FDataField* FClassFields::GetFieldByName(const FString& Name) const
{
	const auto FieldPtr = FieldsByName.Find(Name);
	return FieldPtr ? *FieldPtr : nullptr;
}

const FDataField* FClassFields::GetFieldByAlias(const FString& Alias) const
{
	const auto FieldPtr = FieldsByAlias.Find(Alias);
	return FieldPtr ? *FieldPtr : nullptr;
}

const FDataField* FClassFields::GetFieldByIndex(int32 Index) const
{
	return FieldsList.IsValidIndex(Index) ? FieldsList[Index] : nullptr;
}

const FDataField* FClassFields::GetFieldByHashChecked(int32 Hash) const
{
	return FieldsByHash.FindChecked(Hash);
}

const FDataField* FClassFields::GetFieldByNameChecked(const FString& Name) const
{
	return FieldsByName.FindChecked(Name);
}

const FDataField* FClassFields::GetFieldByAliasChecked(const FString& Alias) const
{
	return FieldsByAlias.FindChecked(Alias);
}

const FDataField* FClassFields::GetFieldByIndexChecked(int32 Index) const
{
	return FieldsList[Index];
}

bool FClassFields::HasFieldWithHash(int32 Hash) const
{
	return FieldsByHash.Contains(Hash);
}

bool FClassFields::HasFieldWithName(const FString& Name) const
{
	return FieldsByName.Contains(Name);
}

bool FClassFields::HasFieldWithAlias(const FString& Alias) const
{
	return HasFieldWithName(Alias) || FieldsByAlias.Contains(Alias);
}

bool FClassFields::HasFieldWithIndex(int32 Index) const
{
	return FieldsList.IsValidIndex(Index);
}

int32 FClassFields::GetNumFields() const
{
	return FieldsList.Num();
}

const FDataLink* FClassFields::GetLinkByHash(int32 Hash) const
{
	const auto LinkPtr = LinksByHash.Find(Hash);
	return LinkPtr ? *LinkPtr : nullptr;
}

const FDataLink* FClassFields::GetLinkByName(const FString& Name) const
{
	const auto LinkPtr = LinksByName.Find(Name);
	return LinkPtr ? *LinkPtr : nullptr;
}

const FDataLink* FClassFields::GetLinkByHashChecked(int32 Hash) const
{
	return LinksByHash.FindChecked(Hash);
}

const FDataLink* FClassFields::GetLinkByNameChecked(const FString& Name) const
{
	return LinksByName.FindChecked(Name);
}

int32 FClassFields::GetNumLinks() const
{
	return LinkList.Num();
}

TMap<UClass*, FClassFields> FDataReflection::FieldsByClass;
TMap<FString, const TArray<FString>> FDataReflection::SplittedPath;
TArray<const char*> FDataReflection::MetaCollection;
bool FDataReflection::bCompiled = false;

void FDataReflection::InitField(const char* CharName, int32 Hash, FAbstractDataTypeContext* Context, FDataField*& Field, UPsData* Instance, FAbstractDataProperty* Property)
{
	if (!bCompiled)
	{
		const FString FieldName(CharName);

		UClass* OwnerClass = Instance->GetClass();
		auto& ClassFields = FieldsByClass.FindOrAdd(OwnerClass);

#if !UE_BUILD_SHIPPING
		if (Field)
		{
			const auto SuperClassFields = FieldsByClass.Find(OwnerClass->GetSuperClass());
			if (!SuperClassFields || SuperClassFields->GetFieldByName(FieldName) != Field)
			{
				UE_LOG(LogData, Fatal, TEXT("Attempting to recreate field %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
			}
		}
		else if (ClassFields.HasFieldWithHash(Hash))
		{
			UE_LOG(LogData, Fatal, TEXT("Can't generate unique hash for property %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
		}
		else if (ClassFields.HasFieldWithName(FieldName))
		{
			UE_LOG(LogData, Fatal, TEXT("Duplicate name for property %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
		}
#endif // !UE_BUILD_SHIPPING

		const int32 Index = ClassFields.GetNumFields();

		if (!Field)
		{
			Field = new FDataField(FieldName, Index, Hash, Context, MetaCollection);
		}

		MetaCollection.Reset();

#if !UE_BUILD_SHIPPING
		if (ClassFields.HasFieldWithAlias(Field->GetAliasName()))
		{
			UE_LOG(LogData, Fatal, TEXT("Duplicate alias for property %s::%s %d"), *OwnerClass->GetName(), *FieldName, Hash);
		}
#endif // !UE_BUILD_SHIPPING

		ClassFields.AddField(Field);

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
		auto& ClassFields = FieldsByClass.FindOrAdd(OwnerClass);

		bool bPathProperty = false;
		if (!bAbstract)
		{
			const auto PathField = ClassFields.GetFieldByName(Path);
			if (PathField)
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

		if (const auto ExistingLink = ClassFields.GetLinkByHash(Hash))
		{
			if (!ExistingLink->bAbstract || (ExistingLink->bAbstract && bAbstract))
			{
				UE_LOG(LogData, Fatal, TEXT("Can't override link for %s::%s %d"), *OwnerClass->GetName(), *ExistingLink->Name, Hash);
			}
		}

		FDataLink* Link = new FDataLink(Name, Path, bPathProperty, ReturnType, Hash, bAbstract, bCollection, MetaCollection);
		MetaCollection.Reset();

		ClassFields.AddLink(Link);
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

		auto& ClassFields = FieldsByClass.FindOrAdd(Class);
		ClassFields.Sort();

		UE_LOG(LogData, VeryVerbose, TEXT("%s complete!"), *Class->GetName())
	}

	const bool bDefaultObject = Instance->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject | EObjectFlags::RF_DefaultSubObject);
	if (bDefaultObject)
	{
		if (!bCompiled)
		{
			UClass* Class = Instance->GetClass();
			auto& ClassFields = FieldsByClass.FindOrAdd(Class);

			FPsDataFriend::InitProperties(Instance);

			for (const auto Property : FPsDataFriend::GetProperties(Instance))
			{
				auto ConstField = Property->GetField();
				if (!Property->IsDefault() || ConstField->Meta.bStrict)
				{
					auto Field = ClassFields.GetMutableField(Property->GetField());
					Field->Meta.bDefault = false;
				}
			}
		}
	}
	else
	{
		auto& Properties = PsDataTools::FPsDataFriend::GetProperties(Instance);
		Properties.Shrink();

		for (const auto Property : Properties)
		{
			const auto Field = Property->GetField();
			if (Field->Meta.bStrict)
			{
				Properties[Field->Index]->Allocate(Instance);
			}
		}

		FPsDataFriend::InitProperties(Instance);
	}
}

const FClassFields* FDataReflection::GetFieldsByClass(const UClass* Class)
{
	auto ClassFields = FieldsByClass.Find(Class);
	if (ClassFields)
	{
		return ClassFields;
	}

	static const FClassFields StaticClassFields;
	return &StaticClassFields;
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
	for (auto& Pair : FieldsByClass)
	{
		for (auto Field : Pair.Value.GetFieldsList())
		{
			if (Field->Meta.bReadOnly)
			{
				ReadOnlyFields.Add(Field->Context->GetUE4Type());
			}
		}
	}

	while (ReadOnlyFields.Num() > 0)
	{
		UField* UEField = ReadOnlyFields.Pop();
		if (UClass* Class = Cast<UClass>(UEField))
		{
			if (auto ClassFields = FieldsByClass.Find(Class))
			{
				for (auto Field : ClassFields->GetFieldsList())
				{
					if (!Field->Meta.bReadOnly)
					{
						Field->Meta.bReadOnly = true;
						if (Field->Context->IsData())
						{
							ReadOnlyFields.AddUnique(Field->Context->GetUE4Type());
						}
					}
				}
			}
		}
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
} // namespace PsDataTools
