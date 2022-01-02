// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsDataCore.h"

#include "PsDataRoot.h"
#include "PsDataStruct.h"
#include "PsDataUtils.h"
#include "Types/PsData_FString.h"
#include "Types/PsData_UPsData.h"

namespace PsDataTools
{
FClassFields::FClassFields()
	: Recursion(0)
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
	LinkList.Add(Link);
	ConstLinkList.Add(Link);

	LinksByHash.Add(Link->Hash, Link);
}

void FClassFields::AddSuper(const FClassFields& SuperFields)
{
	FieldsList.Append(SuperFields.FieldsList);
	ConstFieldsList.Append(SuperFields.ConstFieldsList);

	FieldsByName.Append(SuperFields.FieldsByName);
	FieldsByAlias.Append(SuperFields.FieldsByAlias);
	FieldsByHash.Append(SuperFields.FieldsByHash);

	LinkList.Append(SuperFields.LinkList);
	ConstLinkList.Append(SuperFields.ConstLinkList);

	LinksByHash.Append(SuperFields.LinksByHash);
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

const TArray<FDataField*>& FClassFields::GetFieldsList()
{
	return FieldsList;
}

const TArray<const FDataField*>& FClassFields::GetFieldsList() const
{
	return ConstFieldsList;
}

FDataField* FClassFields::GetFieldByHash(int32 Hash)
{
	const auto FieldPtr = FieldsByHash.Find(Hash);
	return FieldPtr ? *FieldPtr : nullptr;
}

FDataField* FClassFields::GetFieldByName(const FString& Name)
{
	const auto FieldPtr = FieldsByName.Find(Name);
	return FieldPtr ? *FieldPtr : nullptr;
}

FDataField* FClassFields::GetFieldByAlias(const FString& Alias)
{
	const auto FieldPtr = FieldsByAlias.Find(Alias);
	return FieldPtr ? *FieldPtr : nullptr;
}

FDataField* FClassFields::GetFieldByIndex(int32 Index)
{
	return FieldsList.IsValidIndex(Index) ? FieldsList[Index] : nullptr;
}

FDataField* FClassFields::GetFieldByHashChecked(int32 Hash)
{
	return FieldsByHash.FindChecked(Hash);
}

FDataField* FClassFields::GetFieldByNameChecked(const FString& Name)
{
	return FieldsByName.FindChecked(Name);
}

FDataField* FClassFields::GetFieldByAliasChecked(const FString& Alias)
{
	return FieldsByAlias.FindChecked(Alias);
}

FDataField* FClassFields::GetFieldByIndexChecked(int32 Index)
{
	return FieldsList[Index];
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

const TArray<FDataLink*>& FClassFields::GetLinksList()
{
	return LinkList;
}

const TArray<const FDataLink*>& FClassFields::GetLinksList() const
{
	return ConstLinkList;
}

FDataLink* FClassFields::GetLinkByHash(int32 Hash)
{
	const auto LinkPtr = LinksByHash.Find(Hash);
	return LinkPtr ? *LinkPtr : nullptr;
}

FDataLink* FClassFields::GetLinkByHashChecked(int32 Hash)
{
	return LinksByHash.FindChecked(Hash);
}

const FDataLink* FClassFields::GetLinkByHash(int32 Hash) const
{
	const auto LinkPtr = LinksByHash.Find(Hash);
	return LinkPtr ? *LinkPtr : nullptr;
}

const FDataLink* FClassFields::GetLinkByHashChecked(int32 Hash) const
{
	return LinksByHash.FindChecked(Hash);
}

bool FClassFields::HasLinkWithHash(int32 Hash) const
{
	return LinksByHash.Contains(Hash);
}

int32 FClassFields::GetNumLinks() const
{
	return LinkList.Num();
}

void FClassFields::CalculateDependencies(UClass* HeadClass, UClass* MainClass, TSet<UClass*>& OutList) const
{
	if (Recursion > 0)
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Cyclic dependency detected! Head: %s Main: %s"), *HeadClass->GetName(), *MainClass->GetName());
	}

#if !UE_BUILD_SHIPPING
	check(FDataReflection::GetFieldsByClass(MainClass) == this);
#endif

	++Recursion;
	for (const auto Field : FieldsList)
	{
		if (Field->Context->IsData())
		{
			const auto FieldClass = CastChecked<UClass>(Field->Context->GetUEType());
			OutList.Add(FieldClass);
			FDataReflection::GetFieldsByClass(FieldClass)->CalculateDependencies(HeadClass, FieldClass, OutList);
		}
	}

	auto ParentClass = MainClass->GetSuperClass();
	while (!FDataReflection::IsBaseClass(ParentClass))
	{
		OutList.Add(ParentClass);
		ParentClass = ParentClass->GetSuperClass();
	}

	--Recursion;
}

TMap<UClass*, FClassFields> FDataReflection::FieldsByClass;
TMap<const FDataField*, FLinkPathFunction> FDataReflection::LinkPathFunctionByField;
FDataRawMeta FDataReflection::RawMeta;
UClass* FDataReflection::DescribedClass = nullptr;
bool FDataReflection::bCompiled = false;

bool FDataReflection::InitMeta(const char* MetaString)
{
	check(!bCompiled);

	RawMeta.Append(MetaString);
	return true;
}

bool FDataReflection::InitProperty(UClass* Class, const char* Name, FAbstractDataTypeContext* Context, FDataField*& OutField)
{
	check(!bCompiled);
	check(OutField == nullptr);

	check(Class && Class == DescribedClass);

	const auto Hash = ToStringView(Name).GetHash();
	const auto PropertyName = ToFString(Name);
	auto& ClassFields = FieldsByClass.FindChecked(Class);

	if (!IsValidKey(PropertyName))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Illegal name for property %s::%s (%d)"), *Class->GetName(), *PropertyName, Hash);
	}
	if (ClassFields.HasFieldWithHash(Hash))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Can't generate unique hash for property %s::%s (%d)"), *Class->GetName(), *PropertyName, Hash);
	}
	if (ClassFields.HasFieldWithName(PropertyName))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Duplicate name for property %s::%s (%d)"), *Class->GetName(), *PropertyName, Hash);
	}
	if (ClassFields.HasFieldWithAlias(PropertyName))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Duplicate alias for property %s::%s (%d)"), *Class->GetName(), *PropertyName, Hash);
	}

	OutField = new FDataField(PropertyName, ClassFields.GetNumFields(), Hash, Context, RawMeta);

	if (!IsValidKey(OutField->GetAliasName()))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Illegal alias %s for property %s::%s (%d)"), *Class->GetName(), *PropertyName, Hash);
	}
	if (ClassFields.HasFieldWithAlias(OutField->GetAliasName()))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Duplicate alias for property %s::%s (%d)"), *Class->GetName(), *PropertyName, Hash);
	}

	ClassFields.AddField(OutField);

	UE_LOG(LogDataReflection, VeryVerbose, TEXT(" %04d %s %s::%s (%d)"), ClassFields.GetNumFields(), *Context->GetCppType(), *Class->GetName(), *PropertyName, Hash);
	return true;
}

bool FDataReflection::InitLinkProperty(UClass* Class, const char* Name, bool bAbstract, FAbstractDataTypeContext* ReturnContext, FLinkPathFunction PathFunction, FDataLink*& OutLink)
{
	check(!bCompiled);
	check(OutLink == nullptr);

	check(Class && Class == DescribedClass);

	const auto PropertyName = ToFString(Name);

	auto& ClassFields = FieldsByClass.FindChecked(Class);
	const auto Field = ClassFields.GetFieldByNameChecked(PropertyName);
	const auto Hash = bAbstract ? HashCombine(Field->Hash, PSDATA_ABSTRACT_LINK_SALT) : Field->Hash;

	if (ClassFields.HasLinkWithHash(Hash))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Can't generate unique hash for link %s::%s (%d)"), *Class->GetName(), *Field->Name, Hash);
	}

	if (bAbstract)
	{
		check(PathFunction == nullptr);

		PathFunction = [Field](class UPsData* Data, FString& OutPath) {
			if (auto PathFunctionPtr = LinkPathFunctionByField.Find(Field))
			{
				(*PathFunctionPtr)(Data, OutPath);
			}
		};
	}
	else
	{
		check(PathFunction != nullptr);

		if (LinkPathFunctionByField.Contains(Field))
		{
			UE_LOG(LogDataReflection, Fatal, TEXT("Attempting to recreate link %s::%s"), *Class->GetName(), *Field->Name, Hash);
		}

		LinkPathFunctionByField.Add(Field, PathFunction);
	}

	OutLink = new FDataLink(Field, ClassFields.GetNumLinks(), Hash, ReturnContext, PathFunction, bAbstract, RawMeta);
	ClassFields.AddLink(OutLink);

	UE_LOG(LogDataReflection, VeryVerbose, TEXT(" LINK %s %s::%s (%d)"), *ReturnContext->GetCppType(), *Class->GetName(), *PropertyName, Hash);
	return true;
}

void FDataReflection::PreConstruct(UClass* Class)
{
	if (bCompiled || IsBaseClass(Class) || FieldsByClass.Contains(Class))
	{
		return;
	}

	check(DescribedClass == nullptr);

	auto& ClassFields = FieldsByClass.Add(Class);
	const auto SuperClass = Class->GetSuperClass();
	if (!IsBaseClass(SuperClass))
	{
		ClassFields.AddSuper(FieldsByClass.FindChecked(SuperClass));
	}

	UE_LOG(LogDataReflection, VeryVerbose, TEXT("Describe %s:"), *Class->GetName());
	DescribedClass = Class;
}

void FDataReflection::PostConstruct(UClass* Class)
{
	if (bCompiled || IsBaseClass(Class))
	{
		return;
	}

	check(Class == DescribedClass);

	if (RawMeta.Items.Num() > 0)
	{
		UE_LOG(LogDataReflection, Error, TEXT(" %s has unused meta"), *Class->GetName());
		RawMeta.Reset();
	}

	auto& ClassFields = FieldsByClass.FindChecked(Class);
	ClassFields.Sort();

	UPsData* DefaultObject = CastChecked<UPsData>(Class->GetDefaultObject(false));

	FPsDataFriend::InitProperties(DefaultObject);
	for (const auto Field : ClassFields.GetFieldsList())
	{
		const auto Property = FPsDataFriend::GetProperty(DefaultObject, Field->Index);
		if (!Property->IsDefault() || Field->Meta.bStrict)
		{
			UE_LOG(LogDataReflection, VeryVerbose, TEXT("      non-default: %s"), *Field->Name);
			Field->Meta.bDefault = false;
		}
	}

	UE_LOG(LogDataReflection, VeryVerbose, TEXT("%s complete!"), *Class->GetName());
	DescribedClass = nullptr;
}

const FClassFields* FDataReflection::GetFieldsByClass(const UClass* Class)
{
	const auto ClassFieldsPtr = FieldsByClass.Find(Class);
	if (ClassFieldsPtr)
	{
		return ClassFieldsPtr;
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

	check(DescribedClass == nullptr);

	TArray<UClass*> ReadOnlyClasses;
	TMap<UClass*, TSet<UClass*>> UnresolvedDependencies;
	for (auto& Pair : FieldsByClass)
	{
		for (const auto Field : Pair.Value.GetFieldsList())
		{
			if (Field->Meta.bReadOnly && Field->Context->IsData())
			{
				ReadOnlyClasses.Add(CastChecked<UClass>(Field->Context->GetUEType()));
			}
		}

		auto& Dependencies = UnresolvedDependencies.Add(Pair.Key);
		Pair.Value.CalculateDependencies(Pair.Key, Pair.Key, Dependencies);
	}

	for (const auto ReadOnlyClass : ReadOnlyClasses)
	{
		const auto& Dependencies = UnresolvedDependencies.FindChecked(ReadOnlyClass);
		for (const auto Class : Dependencies)
		{
			for (const auto Field : FieldsByClass.FindChecked(Class).GetFieldsList())
			{
				Field->Meta.bReadOnly = true;
			}
		}
	}

	while (UnresolvedDependencies.Num() > 0)
	{
		const int32 NumUnresolvedDependencies = UnresolvedDependencies.Num();
		for (auto ItA = UnresolvedDependencies.CreateIterator(); ItA; ++ItA)
		{
			const auto Class = ItA.Key();
			auto& ClassDependencies = ItA.Value();

			for (auto ItB = ClassDependencies.CreateIterator(); ItB; ++ItB)
			{
				if (!UnresolvedDependencies.Contains(*ItB))
				{
					ItB.RemoveCurrent();
				}
			}

			if (ClassDependencies.Num() == 0)
			{
				CompileClass(ItA.Key());
				ItA.RemoveCurrent();
			}
		}

		if (NumUnresolvedDependencies == UnresolvedDependencies.Num())
		{
			UE_LOG(LogDataReflection, Fatal, TEXT("Can't resolve dependencies"));
		}
	}

	bCompiled = true;
}

void FDataReflection::CompileClass(UClass* Class)
{
	check(!bCompiled);

	UPsData* DefaultObject = CastChecked<UPsData>(Class->GetDefaultObject(false));
	FPsDataFriend::InitStructProperties(DefaultObject);
}

bool FDataReflection::IsBaseClass(const UClass* Class)
{
	return UPsData::StaticClass() == Class || UPsDataRoot::StaticClass() == Class || UObject::StaticClass() == Class;
}

} // namespace PsDataTools
