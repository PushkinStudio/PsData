// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataCore.h"

const FString EDataMetaType::Strict = TEXT("strict");
const FString EDataMetaType::Event = TEXT("event");
const FString EDataMetaType::Bubbles = TEXT("bubbles");
const FString EDataMetaType::Alias = TEXT("alias");
const FString EDataMetaType::Link = TEXT("link");
const FString EDataMetaType::Deprecated = TEXT("deprecated");

IPsDataAccess* FDataReflection::DataAccess = nullptr;
TMap<UClass*, TMap<FString, FDataFieldDescription>> FDataReflection::Fields;
TArray<UClass*> FDataReflection::ClassQueue;
TArray<FString> FDataReflection::MetaCollection;

FString FDataReflection::GetTypeAsString(EDataFieldType Type)
{
	switch(Type)
	{
		case EDataFieldType::DFT_Data:
			return TEXT("Data");
		case EDataFieldType::DFT_float:
			return TEXT("Float");
		case EDataFieldType::DFT_int32:
			return TEXT("Int");
		case EDataFieldType::DFT_String:
			return TEXT("String");
		case EDataFieldType::DFT_bool:
			return TEXT("Bool");
		default:
			UE_LOG(LogData, Error, TEXT("Unsupport type: %d"), (uint8) Type)
			return TEXT("Unsupport");
	}
}

FString FDataReflection::GenerateGetFunctionName(const FDataFieldDescription& Field)
{
	return FString::Printf(TEXT("Get%s%sProperty"), *GetTypeAsString(Field.Type), Field.ContainerType == EDataContainerType::DCT_None ? TEXT("") : (Field.ContainerType == EDataContainerType::DCT_Array ? TEXT("Array") : TEXT("Map")));
}

FString FDataReflection::GenerateSetFunctionName(const FDataFieldDescription& Field)
{
	return FString::Printf(TEXT("Set%s%sProperty"), *GetTypeAsString(Field.Type), Field.ContainerType == EDataContainerType::DCT_None ? TEXT("") : (Field.ContainerType == EDataContainerType::DCT_Array ? TEXT("Array") : TEXT("Map")));
}

FString FDataReflection::GenerateChangePropertyEventTypeName(const FDataFieldDescription& Field)
{
	if (!Field.Meta.EventType.IsEmpty())
	{
		return Field.Meta.EventType;
	}
	return FString::Printf(TEXT("%sСhanged"), *Field.Name);
}

void FDataReflection::AddField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataFieldType Type, EDataContainerType ContainerType)
{
	FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(Type, ContainerType, Name, Offset, Size, MetaCollection));
}

void FDataReflection::AddField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, UClass* Type, EDataContainerType ContainerType)
{
	FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(Type, ContainerType, Name, Offset, Size, MetaCollection));
}

void FDataReflection::AddToQueue(UPsData* Instance)
{
	UClass* Class = Instance->GetClass();
	if (UPsData::StaticClass() == Class)
	{
		return;
	}
	
	if (Fields.Contains(Class))
	{
		return;
	}
	
	if ((Class->GetClassFlags() & CLASS_Constructed) == 0)
	{
		return;
	}
	
	UClass* SuperClass = Class->GetSuperClass();
	if (Fields.Contains(SuperClass))
	{
		Fields.FindOrAdd(Class).Append(*Fields.Find(SuperClass));
	}
	else
	{
		Fields.FindOrAdd(Class);
	}
	
	ClassQueue.Add(Class);
	UE_LOG(LogData, Verbose, TEXT("Describe %s start"), *Class->GetName())
}

void FDataReflection::RemoveFromQueue(UPsData* Instance)
{
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
		
		UE_LOG(LogData, Verbose, TEXT("Describe %s complete"), *Class->GetName())
	}
}

void FDataReflection::Fill(UPsData* Instance)
{
	const TMap<FString, FDataFieldDescription>& FieldMap = FDataReflection::GetFields(Instance->GetClass());
	for (auto& Pair : FieldMap)
	{
		if (Pair.Value.Type == EDataFieldType::DFT_Data && Pair.Value.ContainerType == EDataContainerType::DCT_None)
		{
			UPsData*& Data = *FDataReflectionTools::UnsafeGet<UPsData*>(Instance, Pair.Value.Offset);
			Data = nullptr;
			
			if (Instance->HasAnyFlags(EObjectFlags::RF_ClassDefaultObject | EObjectFlags::RF_DefaultSubObject))
			{
				continue;
			}
			
			if (Pair.Value.Meta.bStrict)
			{
				UPsData* NewObject = ::NewObject<UPsData>(Instance, Pair.Value.Class);
				FDataReflectionTools::Set<UPsData*>(Instance, Pair.Value.Name, NewObject);
			}
		}
	}
}

bool FDataReflection::InQueue(UClass* StaticClass)
{
	return ClassQueue.Num() > 0 && ClassQueue.Last() == StaticClass;
}

bool FDataReflection::InQueue()
{
	return ClassQueue.Num() > 0;
}

UClass* FDataReflection::GetLastClassInQueue()
{
	if (ClassQueue.Num() > 0)
	{
		return ClassQueue.Last();
	}
	return nullptr;
}

bool FDataReflection::HasClass(const UClass* StaticClass)
{
	return Fields.Contains(StaticClass);
}

const TMap<FString, FDataFieldDescription>& FDataReflection::GetFields(const UClass* StaticClass)
{
	auto Find = Fields.Find(StaticClass);
	if (Find)
	{
		return *Find;
	}
	
	static TMap<FString, FDataFieldDescription> Empty;
	return Empty;
}

void ParseValue(FString& Value)
{
	if (Value.IsEmpty())
	{
		return;
	}
	
	Value.TrimStartAndEndInline();
	
	if (Value.IsEmpty())
	{
		return;
	}
	
	int32 Last = Value.Len() - 1;
	if (Last != 0 && ((Value[0] == '"' && Value[Last] == '"') || (Value[0] == '\'' && Value[Last] == '\'')))
	{
		Value = Value.Mid(1, Value.Len() - 2);
	}
}

void FDataFieldDescription::ParseMeta(TArray<FString>& Collection)
{
	for(FString& Item : Collection)
	{
		FString Key;
		FString Value;
		if (!Item.Split(TEXT("="), &Key, &Value))
		{
			Key = Item;
		}
		Key.TrimStartAndEndInline();
		Key.ToLowerInline();
		ParseValue(Value);
		
		if (Key.Equals(EDataMetaType::Strict, ESearchCase::CaseSensitive))
		{
			Meta.bStrict = true;
		}
		else if (Key.Equals(EDataMetaType::Event, ESearchCase::CaseSensitive))
		{
			Meta.bEvent = true;
			Meta.EventType = Value;
		}
		else if (Key.Equals(EDataMetaType::Bubbles, ESearchCase::CaseSensitive))
		{
			Meta.bBubbles = true;
		}
		else if (Key.Equals(EDataMetaType::Alias, ESearchCase::CaseSensitive))
		{
			Meta.Alias = Value;
		}
		else if (Key.Equals(EDataMetaType::Link, ESearchCase::CaseSensitive))
		{
			Meta.bLink = true;
			Meta.LinkPath = Value;
		}
		else if (Key.Equals(EDataMetaType::Deprecated, ESearchCase::CaseSensitive))
		{
			Meta.bDeprecated = true;
		}
		else
		{
			UE_LOG(LogData, Error, TEXT("Unknown meta \"%s\" in %s::%s (%s)"), *Key, *FDataReflection::GetLastClassInQueue()->GetName(), *Name, *Item)
		}
	}
	
	if (Meta.bStrict && Meta.bEvent)
	{
		Meta.bEvent = false;
		Meta.bBubbles = false;
		Meta.EventType = TEXT("");
		
		UE_LOG(LogData, Error, TEXT("Property %s::%s with strict meta can't broadcast event"), *FDataReflection::GetLastClassInQueue()->GetName(), *Name)
	}
	
	if (ContainerType != EDataContainerType::DCT_None)
	{
		if (Meta.bStrict)
		{
			Meta.bStrict = false;
			UE_LOG(LogData, Error, TEXT("Container %s::%s can't have strict meta"), *FDataReflection::GetLastClassInQueue()->GetName(), *Name)
		}
	}
	
	Collection.Reset();
}

void FDataReflection::DeclareMeta(FString Meta)
{
	if (!Meta.Contains(TEXT(",")))
	{
		Meta.TrimStartAndEndInline();
		MetaCollection.Add(Meta);
	}
	else
	{
		TArray<FString> Collection;
		Meta.ParseIntoArray(Collection, TEXT(","), true);
		MetaCollection.Reserve(MetaCollection.Num() + Collection.Num());
		for(FString& Item : Collection)
		{
			Item.TrimStartAndEndInline();
			MetaCollection.Add(Item);
		}
	}
}

void FDataReflection::ClearMeta()
{
	MetaCollection.Reset();
}

void FDataReflection::SetDataAccess(IPsDataAccess* DataAccessInterface)
{
	check(DataAccessInterface)
	DataAccess = DataAccessInterface;
}

IPsDataAccess* FDataReflection::GetDataAccess()
{
	return DataAccess;
}
