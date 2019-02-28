// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataCore.h"

#include "UObject/UObjectIterator.h"

TMap<UClass*, TMap<FString, TSharedPtr<const FDataField>>> FDataReflection::FieldsByName;
TMap<UClass*, TMap<int32, TSharedPtr<const FDataField>>> FDataReflection::FieldsByHash;

TArray<UClass*> FDataReflection::ClassQueue;
TArray<const char*> FDataReflection::MetaCollection;

bool FDataReflection::bCompiled = false;

void FDataReflection::AddField(UClass* OwnerClass, const FString& Name, int32 Hash, FAbstractDataTypeContext* Context)
{
	check(!bCompiled);

	if (!InQueue(OwnerClass))
	{
		if (InQueue())
		{
			UE_LOG(LogData, Error, TEXT("Can't describe: %s::%s another class is active: %s"), *OwnerClass->GetName(), *Name, *GetLastClassInQueue()->GetName());
		}
		else
		{
			UE_LOG(LogData, Error, TEXT("Can't describe: %s::%s because queue is empty"), *OwnerClass->GetName(), *Name);
		}
		return;
	}

	TMap<FString, TSharedPtr<const FDataField>>& MapByName = FieldsByName.FindOrAdd(OwnerClass);
	TMap<int32, TSharedPtr<const FDataField>>& MapByHash = FieldsByHash.FindOrAdd(OwnerClass);

	const int32 Index = MapByName.Num();

	TSharedPtr<const FDataField> Field(new FDataField(Name, Index, Hash, Context, MetaCollection));
	MetaCollection.Reset();

	if (MapByHash.Contains(Hash))
	{
		UE_LOG(LogData, Fatal, TEXT("Can't generate unique hash for %s::%s 0x%08x"), *OwnerClass->GetName(), *Name, Hash);
	}

	MapByName.Add(Name, Field);
	MapByHash.Add(Hash, Field);

	UE_LOG(LogData, Verbose, TEXT(" %02d %s %s::%s (0x%08x)"), Index + 1, *Context->GetCppType(), *OwnerClass->GetName(), *Name, Hash);
}

void FDataReflection::AddToQueue(UPsData* Instance)
{
	if (bCompiled)
	{
		return;
	}

	UClass* Class = Instance->GetClass();
	if (UPsData::StaticClass() == Class)
	{
		return;
	}

	if (FieldsByName.Contains(Class))
	{
		return;
	}

	if ((Class->GetClassFlags() & CLASS_Constructed) == 0)
	{
		return;
	}

	UClass* SuperClass = Class->GetSuperClass();
	if (FieldsByName.Contains(SuperClass))
	{
		FieldsByName.FindOrAdd(Class).Append(*FieldsByName.Find(SuperClass));
	}
	else
	{
		FieldsByName.FindOrAdd(Class);
	}

	ClassQueue.Add(Class);
	UE_LOG(LogData, Verbose, TEXT("Describe %s:"), *Class->GetName())
}

void FDataReflection::RemoveFromQueue(UPsData* Instance)
{
	if (bCompiled)
	{
		return;
	}

	if (ClassQueue.Num() > 0 && ClassQueue.Last() == Instance->GetClass())
	{
		if (ClassQueue.Num() == 1)
		{
			ClassQueue.SetNum(0, true);
		}
		else
		{
			ClassQueue.SetNum(ClassQueue.Num() - 1, false);
		}

		UClass* Class = Instance->GetClass();

		if (MetaCollection.Num() > 0)
		{
			UE_LOG(LogData, Error, TEXT(" %s has unused meta"), *Class->GetName())
			MetaCollection.Reset();
		}

		// Sort by Hash
		FieldsByHash.FindOrAdd(Class).KeySort([](const int32& a, const int32& b) -> bool {
			return a < b;
		});

		//Sort by String
		FieldsByName.FindOrAdd(Class).KeySort([](const FString& a, const FString& b) -> bool {
			return a < b;
		});

		UE_LOG(LogData, Verbose, TEXT("%s complete!"), *Class->GetName())
	}
}

bool FDataReflection::InQueue(UClass* StaticClass)
{
	return !bCompiled && ClassQueue.Num() > 0 && ClassQueue.Last() == StaticClass;
}

bool FDataReflection::InQueue()
{
	return !bCompiled && ClassQueue.Num() > 0;
}

UClass* FDataReflection::GetLastClassInQueue()
{
	if (!bCompiled && ClassQueue.Num() > 0)
	{
		return ClassQueue.Last();
	}
	return nullptr;
}
void FDataReflection::PushMeta(const char* Meta)
{
	check(!bCompiled);
	MetaCollection.Add(Meta);
}

void FDataReflection::ClearMeta()
{
	check(!bCompiled);
	MetaCollection.Reset();
}

void FDataReflection::Fill(UPsData* Instance)
{
	const bool bDefaultObject = Instance->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject | EObjectFlags::RF_DefaultSubObject);

	const TMap<FString, TSharedPtr<const FDataField>>& FieldMap = FDataReflection::GetFields(Instance->GetClass());

	auto& Memory = FDataReflectionTools::FPsDataFriend::GetMemory(Instance);
	Memory.AddDefaulted(FieldMap.Num());

	for (const auto& Pair : FieldMap)
	{
		const auto& Field = Pair.Value;
		Memory[Field->Index].Reset(Field->Context->AllocateMemory());

		if (Field->Context->IsData() && !Field->Context->IsContainer())
		{
			UPsData** Data = nullptr;
			FDataReflectionTools::GetByField<UPsData*>(Instance, Field, Data);
			*Data = nullptr;

			if (bDefaultObject)
			{
				continue;
			}

			UClass* DataType = Cast<UClass>(Field->Context->GetUE4Type());
			if (Field->Meta.bStrict && DataType != nullptr)
			{
				UPsData* NewObject = ::NewObject<UPsData>(Instance, Cast<UClass>(DataType));
				FDataReflectionTools::SetByField<UPsData*>(Instance, Field, NewObject);
			}
		}
	}
	//TODO: invoke init constructor
}

TSharedPtr<const FDataField> FDataReflection::GetFieldByName(UClass* OwnerClass, const FString& Name)
{
	TMap<FString, TSharedPtr<const FDataField>>* MapPtr = FieldsByName.Find(OwnerClass);
	if (MapPtr)
	{
		TSharedPtr<const FDataField>* SharedPtr = MapPtr->Find(Name);
		if (SharedPtr)
		{
			return *SharedPtr;
		}
	}

	return nullptr;
}

TSharedPtr<const FDataField> FDataReflection::GetFieldByHash(UClass* OwnerClass, int32 Hash)
{
	if (auto MapPtr = FieldsByHash.Find(OwnerClass))
	{
		if (auto FieldPtr = MapPtr->Find(Hash))
		{
			return *FieldPtr;
		}
	}
	return nullptr;
}

bool FDataReflection::HasClass(const UClass* StaticClass)
{
	return FieldsByName.Contains(StaticClass);
}

const TMap<FString, TSharedPtr<const FDataField>>& FDataReflection::GetFields(const UClass* StaticClass)
{
	auto Find = FieldsByName.Find(StaticClass);
	if (Find)
	{
		return *Find;
	}

	static TMap<FString, TSharedPtr<const FDataField>> Empty;
	return Empty;
}

void FDataReflection::Compile()
{
	check(!bCompiled);
	bCompiled = true;

	ClassQueue.Shrink();
	MetaCollection.Shrink();

	TArray<UField*> ReadOnlyFields;
	for (auto& MapPair : FieldsByHash)
	{
		for (auto& Pair : MapPair.Value)
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
		if (UClass* UEClass = Cast<UClass>(UEField))
		{
			auto Find = FieldsByHash.Find(UEClass);
			if (Find)
			{
				auto& Map = *Find;
				for (auto& Pair : Map)
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
