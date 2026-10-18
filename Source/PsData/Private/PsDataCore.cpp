// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataCore.h"

#include "PsDataRoot.h"
#include "PsDataStruct.h"
#include "PsDataUtils.h"
#include "PsNetworkData.h"
#include "Types/PsData_FString.h"
#include "Types/PsData_UPsData.h"

static FAutoConsoleCommand CmdPsDataTypeInfo(
	TEXT("PsData.TypeInfo"),
	TEXT("Print type information"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) {
		if (Args.Num() == 1)
		{
			PsDataTools::FDataReflection::PrintTypeInfo(Args[0]);
		}
	}),
	ECVF_Cheat);

namespace PsDataTools
{
FClassFields::FClassFields()
	: Recursion(0)
{
}

FClassFields::~FClassFields()
{
	FieldsList.Empty();
	ConstFieldsList.Empty();

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

void FClassFields::CalculateDependencies(UClass* HeadClass, UClass* MainClass, TSet<UClass*>& OutList, bool& bOutHasCycDep) const
{
	if (Recursion > 0)
	{
		bOutHasCycDep = true;
		return;
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
			if (HeadClass == FieldClass && Field->Meta.bStrict)
			{
				UE_LOG(LogDataReflection, Fatal, TEXT("Cyclic allocation detected (strict meta detected) for field: %s Head: %s Main: %s"), *Field->Name, *HeadClass->GetName(), *MainClass->GetName());
			}

			OutList.Add(FieldClass);
			FDataReflection::GetFieldsByClass(FieldClass)->CalculateDependencies(HeadClass, FieldClass, OutList, bOutHasCycDep);
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
	const auto PropertyName = ToString(Name);
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
		UE_LOG(LogDataReflection, Fatal, TEXT("Illegal alias %s for property %s::%s (%d)"), *OutField->GetAliasName(), *Class->GetName(), *PropertyName, Hash);
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

	const auto PropertyName = ToString(Name);

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
			UE_LOG(LogDataReflection, Fatal, TEXT("Attempting to recreate link %s::%s (%d)"), *Class->GetName(), *Field->Name, Hash);
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

bool FDataReflection::IsCompiled()
{
	return bCompiled;
}

void FDataReflection::Compile()
{
	check(!bCompiled);

	check(DescribedClass == nullptr);

	TSet<UClass*> ClassesWithCycDep;
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

		bool bHasCycDep = false;
		Pair.Value.CalculateDependencies(Pair.Key, Pair.Key, Dependencies, bHasCycDep);

		if (bHasCycDep)
		{
			ClassesWithCycDep.Add(Pair.Key);
		}
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
			const auto bHasCycDep = ClassesWithCycDep.Contains(Class);
			auto& ClassDependencies = ItA.Value();

			if (bHasCycDep)
			{
				ClassDependencies.Remove(Class);
			}

			for (auto ItB = ClassDependencies.CreateIterator(); ItB; ++ItB)
			{
				if (!UnresolvedDependencies.Contains(*ItB))
				{
					ItB.RemoveCurrent();
				}
			}

			if (ClassDependencies.Num() == 0)
			{
				CompileClass(Class, bHasCycDep);
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

void FDataReflection::CompileClass(UClass* Class, bool bHasCycDep)
{
	check(!bCompiled);

	const auto Instance = NewObject<UPsData>(GetTransientPackage(), Class);
	const bool bGenerateStruct = FPsDataFriend::ShouldBeGenerateStruct(Instance);
	if (bGenerateStruct && bHasCycDep)
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Can't generate struct for class %s with cyclic dependency"), *Class->GetName());
	}

	if (!bHasCycDep)
	{
		CompileClassInstance(Instance, bGenerateStruct);
	}
}

void FDataReflection::CompileClassInstance(UPsData* Instance, bool bGenerateStruct)
{
	FPsDataFriend::InitProperties(Instance);

	for (const auto Property : FPsDataFriend::GetProperties(Instance))
	{
		const auto Field = Property->GetField();
		if (Field->Context->IsData() && !Field->Context->IsContainer())
		{
			UPsData** ChildPtr = nullptr;
			GetByField<true>(Instance, Field, ChildPtr);

			if (*ChildPtr == nullptr)
			{
				Property->Allocate();
				check(*ChildPtr);
			}

			CompileClassInstance(*ChildPtr, bGenerateStruct);
		}
	}

	if (bGenerateStruct && !FPsDataFriend::ShouldBeGenerateStruct(Instance))
	{
		if (!UPsDataStruct::Find(Instance->GetClass()))
		{
			UPsDataStruct::Create(Instance->GetClass(), Instance);
		}
	}
	else
	{
		FPsDataFriend::InitStructProperties(Instance);
	}
}

void FDataReflection::PrintTypeInfo(const FString& Type)
{
	if (Type.Len() > 0)
	{
		const auto Class = FindUClass(&Type[1]);
		const auto Fields = GetFieldsByClass(Class);
		if (Fields)
		{
			if (Fields->GetNumFields() > 0)
			{
				UE_LOG(LogDataReflection, Display, TEXT("Fields:"));

				for (const auto Field : Fields->GetFieldsList())
				{
					UE_LOG(LogDataReflection, Display, TEXT(" %s U%s::%s"), *Field->Context->GetCppType(), *Class->GetName(), *Field->Name);
				}
			}

			if (Fields->GetNumLinks() > 0)
			{
				UE_LOG(LogDataReflection, Display, TEXT(" "));
				UE_LOG(LogDataReflection, Display, TEXT("Links:"));

				for (const auto Link : Fields->GetLinksList())
				{
					UE_LOG(LogDataReflection, Display, TEXT(" %s U%s::%s"), *Link->ReturnContext->GetCppType(), *Class->GetName(), *Link->Field->Name);
				}
			}

			{
				const auto Chains = GetParentChain(Class, true, false);
				if (Chains.Num() > 0)
				{
					UE_LOG(LogDataReflection, Display, TEXT(" "));
					UE_LOG(LogDataReflection, Display, TEXT("Field paths:"));
				}

				TArray<FString> List;
				List.Reserve(Chains.Num());

				for (const auto& Chain : Chains)
				{
					FString Path;

					for (const auto Pair : Chain)
					{
						if (Path.Len() > 0)
						{
							Path = Pair.Value->Name + TEXT(".") + Path;
						}
						else
						{
							Path = Pair.Value->Name + Path;
						}
					}

					List.Add(FString::Printf(TEXT(" U%s: %s"), *Chain.Last().Key->GetName(), *Path));
				}

				List.Sort([](const FString& A, const FString& B) {
					return A < B;
				});

				for (const auto& Path : List)
				{
					UE_LOG(LogDataReflection, Display, TEXT("%s"), *Path);
				}
			}

			{
				const auto Chains = GetParentChain(Class, false, true);

				if (Chains.Num() > 0)
				{
					UE_LOG(LogDataReflection, Display, TEXT(" "));
					UE_LOG(LogDataReflection, Display, TEXT("Link paths:"));
				}

				TArray<FString> List;
				List.Reserve(Chains.Num());

				for (const auto& Chain : Chains)
				{
					FString Path;

					for (const auto Pair : Chain)
					{
						if (Path.Len() > 0)
						{
							Path = Pair.Value->Name + TEXT(".") + Path;
						}
						else
						{
							Path = Pair.Value->Name + Path;
						}
					}

					List.Add(FString::Printf(TEXT(" U%s: %s"), *Chain.Last().Key->GetName(), *Path));
				}

				List.Sort([](const FString& A, const FString& B) {
					return A < B;
				});

				for (const auto& Path : List)
				{
					UE_LOG(LogDataReflection, Display, TEXT("%s"), *Path);
				}
			}

			return;
		}
	}

	UE_LOG(LogDataReflection, Display, TEXT("Unknown type: %s"), *Type);
}

TArray<FDataReflection::FFieldPair> FDataReflection::GetParentFieldForClass(const UClass* Class)
{
	TArray<FFieldPair> Result;
	for (const auto& Pair : FieldsByClass)
	{
		for (const auto Field : Pair.Value.GetFieldsList())
		{
			if (Field->Context->GetUEType() == Class)
			{
				Result.Add(FFieldPair(Pair.Key, Field));
			}
		}
	}

	return Result;
}

TArray<FDataReflection::FLinkPair> FDataReflection::GetParentLinkForClass(const UClass* Class)
{
	TArray<FLinkPair> Result;
	for (const auto& Pair : FieldsByClass)
	{
		for (const auto Link : Pair.Value.GetLinksList())
		{
			if (Link->ReturnContext->GetUEType() == Class)
			{
				Result.Add(FLinkPair(Pair.Key, Link));
			}
		}
	}
	return Result;
}

TArray<TArray<FDataReflection::FFieldPair>> FDataReflection::GetParentChain(const UClass* Class, bool bFields, bool bLinks)
{
	TArray<TArray<FFieldPair>> List;
	if (bFields)
	{
		for (auto Pair : GetParentLinkForClass(Class))
		{
			List.AddDefaulted_GetRef().Add(FFieldPair(Pair.Key, Pair.Value->Field));
		}
	}

	if (bLinks)
	{
		for (auto Pair : GetParentLinkForClass(Class))
		{
			List.AddDefaulted_GetRef().Add(FFieldPair(Pair.Key, Pair.Value->Field));
		}
	}

	GetParentChainInternal(List);

	return List;
}

void FDataReflection::GetParentChainInternal(TArray<TArray<FFieldPair>>& InOutList)
{
	for (int32 i = 0; i < InOutList.Num(); ++i)
	{
		auto& Pairs = InOutList[i];
		const auto ParentFields = GetParentFieldForClass(Pairs.Last().Key);
		if (ParentFields.Num() > 0)
		{
			for (int32 j = 1; j < ParentFields.Num(); ++j)
			{
				auto NewPairs = Pairs;
				NewPairs.Add(ParentFields[j]);
				InOutList.Add(NewPairs);
			}

			Pairs.Add(ParentFields[0]);
			i -= 1;
		}
	}
}

bool FDataReflection::IsBaseClass(const UClass* Class)
{
	return UPsData::StaticClass() == Class || UPsDataRoot::StaticClass() == Class || UObject::StaticClass() == Class || UPsNetworkData::StaticClass() == Class;
}

} // namespace PsDataTools
