// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Serialize/PsDataSchemaJson.h"

#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataDefines.h"

#include "CoreUObject.h"
#include "Dom/JsonObject.h"

namespace
{
TSharedPtr<FJsonObject> MakePsDataChildJson(const FString Name, const FAbstractDataTypeContext* Context)
{
	auto ChildPtr = MakeShared<FJsonObject>();
	ChildPtr->SetStringField(TEXT("Name"), Name);

	ChildPtr->SetBoolField(TEXT("IsArray"), Context->IsArray());
	ChildPtr->SetBoolField(TEXT("IsMap"), Context->IsMap());
	ChildPtr->SetBoolField(TEXT("IsContainer"), Context->IsContainer());
	ChildPtr->SetBoolField(TEXT("IsData"), Context->IsData());

	ChildPtr->SetStringField(TEXT("CppType"), Context->GetCppType());
	ChildPtr->SetStringField(TEXT("CppContentType"), Context->GetCppContentType());

	return ChildPtr;
}

TSharedPtr<FJsonObject> MakePsDataClassJson(const UClass* Class)
{
	auto PsDataPtr = MakeShared<FJsonObject>();
	PsDataPtr->SetStringField(TEXT("CppType"), TEXT("U") + Class->GetName());

	TArray<TSharedPtr<FJsonValue>> Children;
	for (const auto Field : PsDataTools::FDataReflection::GetFieldsByClass(Class)->GetFieldsList())
	{
		auto ChildPtr = MakePsDataChildJson(Field->Name, Field->Context);
		Children.Add(MakeShared<FJsonValueObject>(ChildPtr));
	}
	PsDataPtr->SetArrayField(TEXT("Children"), Children);

	return PsDataPtr;
}

TArray<UEnum*> CollectEnums(const UClass* Class)
{
	TArray<UEnum*> Result;

	for (const auto Field : PsDataTools::FDataReflection::GetFieldsByClass(Class)->GetFieldsList())
	{
		const auto& Context = Field->Context;
		if (UField* FieldUE4Type = Context->GetUEType())
		{
			if (UEnum* FieldEnum = Cast<UEnum>(FieldUE4Type))
			{
				UE_LOG(LogData, Log, TEXT("%s UEnum '%s'"), *PS_FUNC_LINE, *FieldEnum->GetName());
				Result.Add(FieldEnum);
			}
		}
	}

	return Result;
}

TArray<TSharedPtr<FJsonValue>> DescribeEnums(const TArray<UEnum*>& PsUEnums)
{
	TArray<TSharedPtr<FJsonValue>> Result;
	TSet<FString> NamesSet;

	for (const auto& DataEnum : PsUEnums)
	{
		const auto EnumName = DataEnum->GetName();
		if (NamesSet.Contains(EnumName))
		{
			UE_LOG(LogData, Log, TEXT("%s skip '%s'"), *PS_FUNC_LINE, *EnumName);
			continue;
		}
		NamesSet.Add(EnumName);

		auto EnumJsonPtr = MakeShared<FJsonObject>();
		EnumJsonPtr->SetStringField(TEXT("Name"), EnumName);

		TArray<TSharedPtr<FJsonValue>> Values;
		for (int32 idx = 0;; ++idx)
		{
			const FString SValue = DataEnum->GetNameStringByIndex(idx);
			if (SValue == TEXT(""))
				break;

			const int64 IValue = DataEnum->GetValueByIndex(idx);
			if (IValue == DataEnum->GetMaxEnumValue())
				continue;

			UE_LOG(LogData, Log, TEXT("%s '%s' value %s=%d"), *PS_FUNC_LINE, *EnumName, *SValue, IValue);

			auto ValuePtr = MakeShared<FJsonObject>();
			ValuePtr->SetStringField("Name", SValue);
			ValuePtr->SetNumberField("Value", IValue);
			Values.Add(MakeShared<FJsonValueObject>(ValuePtr));
		}
		EnumJsonPtr->SetArrayField(TEXT("Values"), Values);

		Result.Add(MakeShared<FJsonValueObject>(EnumJsonPtr));
	}

	return Result;
}
} // namespace

//////////////////////////////////////////////////////////////////////////
// Iterates through all UClasses.
// Generates PsData and UEnums json description.

TSharedPtr<FJsonObject> FPsDataSchemaJson::Get()
{
	auto Result = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> PsDatas;
	TArray<UEnum*> PsUEnums;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		if (!Class->IsChildOf(UPsData::StaticClass()))
			continue;

		UE_LOG(LogData, Log, TEXT("%s PsDataSchema create '%s'"), *PS_FUNC_LINE, *Class->GetName());

		// PsData should be known to the reflection (except PsData itself)
		if (!PsDataTools::FDataReflection::HasClass(Class))
			continue;

		auto PsDataPtr = MakePsDataClassJson(Class);
		PsDatas.Add(MakeShared<FJsonValueObject>(PsDataPtr));

		// We will remove dups later
		PsUEnums.Append(CollectEnums(Class));
	}

	Result->SetArrayField(TEXT("PsDatas"), PsDatas);
	Result->SetArrayField(TEXT("PsEnums"), DescribeEnums(PsUEnums));

	return Result;
}
