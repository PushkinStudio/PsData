// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataField.h"

#include "PsData.h"
#include "PsDataCore.h"

using namespace PsDataTools;

/***********************************
 * FDataMetaType
 ***********************************/

const FDataStringViewChar FDataMetaType::Strict = "strict";
const FDataStringViewChar FDataMetaType::Event = "event";
const FDataStringViewChar FDataMetaType::Bubbles = "bubbles";
const FDataStringViewChar FDataMetaType::Alias = "alias";
const FDataStringViewChar FDataMetaType::ReadOnly = "readonly";
const FDataStringViewChar FDataMetaType::Deprecated = "deprecated";
const FDataStringViewChar FDataMetaType::Nullable = "nullable";
const FDataStringViewChar FDataMetaType::Hidden = "hidden";

/***********************************
 * FDataRawMeta
 ***********************************/

FDataRawMeta::FDataRawMeta()
{
}

void FDataRawMeta::Append(const char* MetaString)
{
	auto MetaStringView = PsDataTools::ToStringView(MetaString);
	while (MetaStringView.Len() > 0)
	{
		auto MetaExpression = MetaStringView.LeftByChar(',');
		Items.Add({MetaExpression.LeftByChar('=').Trim().TrimQuotes(), MetaExpression.RightByChar('=').Trim().TrimQuotes()});
		MetaStringView.RightChopInline(MetaExpression.Len() + 1);
	}
}

void FDataRawMeta::Append(const FDataRawMeta& OtherMeta)
{
	Items.Append(OtherMeta.Items);
}

void FDataRawMeta::Reset()
{
	Items.Reset();
}

FDataRawMetaItem* FDataRawMeta::Find(const FDataStringViewChar& Key)
{
	for (auto& Item : Items)
	{
		if (Item.Key.Equal<true>(Key))
		{
			return &Item;
		}
	}

	return nullptr;
}

const FDataRawMetaItem* FDataRawMeta::Find(const FDataStringViewChar& Key) const
{
	for (auto& Item : Items)
	{
		if (Item.Key.Equal<true>(Key))
		{
			return &Item;
		}
	}

	return nullptr;
}

bool FDataRawMeta::Contains(const FDataStringViewChar& Key) const
{
	return Find(Key) != nullptr;
}

bool FDataRawMeta::Remove(const FDataStringViewChar& Key)
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].Key.Equal<true>(Key))
		{
			Items.RemoveAt(i, 1, false);
			return true;
		}
	}

	return false;
}

/***********************************
 * FDataFieldMeta
 ***********************************/

FDataFieldMeta::FDataFieldMeta()
	: bStrict(false)
	, bEvent(false)
	, bBubbles(false)
	, bDeprecated(false)
	, bReadOnly(false)
	, bAlias(false)
	, bDefault(true)
	, bHidden(false)
{
}

/***********************************
 * FDataLinkMeta
 ***********************************/

FDataLinkMeta::FDataLinkMeta()
	: bNullable(false)
{
}

/***********************************
 * FDataFieldFunctions
 ***********************************/

FDataFieldFunctions::FDataFieldFunctions(UClass* InClass, FName InGetFunctionName, FName InSetFunctionName)
	: Class(InClass)
	, GetFunctionName(InGetFunctionName)
	, SetFunctionName(InSetFunctionName)
{
}

FDataFieldFunctions::FDataFieldFunctions(UClass* InClass, EDataFieldType FieldType)
	: Class(InClass)
{
	switch (FieldType)
	{
	case EDataFieldType::VALUE:
	{
		GetFunctionName = "GetProperty";
		SetFunctionName = "SetProperty";
	}
	break;
	case EDataFieldType::ARRAY:
	{
		GetFunctionName = "GetArrayProperty";
		SetFunctionName = "SetArrayProperty";
	}
	break;
	case EDataFieldType::MAP:
	{
		GetFunctionName = "GetMapProperty";
		SetFunctionName = "SetMapProperty";
	}
	break;
	default:
		checkNoEntry();
		break;
	}
}

UFunction* FDataFieldFunctions::ResolveGetFunction() const
{
	const auto Result = Class->FindFunctionByName(GetFunctionName);
	check(Result);
	return Result;
}

UFunction* FDataFieldFunctions::ResolveSetFunction() const
{
	const auto Result = Class->FindFunctionByName(SetFunctionName);
	check(Result);
	return Result;
}

FDataLinkFunctions::FDataLinkFunctions(UClass* InClass, FName InFunctionName)
	: Class(InClass)
	, FunctionName(InFunctionName)
{
}

FDataLinkFunctions::FDataLinkFunctions(UClass* InClass, EDataFieldType FieldType)
	: Class(InClass)
{
	switch (FieldType)
	{
	case EDataFieldType::VALUE:
		FunctionName = "GetLinkValue";
		break;
	case EDataFieldType::ARRAY:
		FunctionName = "GetArrayLinkValue";
		break;
	default:
		FunctionName = "Unknown";
		break;
	}
}

UFunction* FDataLinkFunctions::ResolveFunction() const
{
	const auto Result = Class->FindFunctionByName(FunctionName);
	check(Result);
	return Result;
}

/***********************************
 * FAbstractDataTypeContext
 ***********************************/

UField* FAbstractDataTypeContext::GetUEType() const
{
	return nullptr;
}

bool FAbstractDataTypeContext::IsArray() const
{
	return false;
}

bool FAbstractDataTypeContext::IsMap() const
{
	return false;
}

bool FAbstractDataTypeContext::IsContainer() const
{
	return IsArray() || IsMap();
}

bool FAbstractDataTypeContext::IsData() const
{
	return false;
}

bool FAbstractDataTypeContext::IsEnum() const
{
	return false;
}

bool FAbstractDataTypeContext::HasExtendedTypeCheck() const
{
	return false;
}

bool FAbstractDataTypeContext::IsA(const FAbstractDataTypeContext* RightContext) const
{
	const bool bSuccess = (GetHash() == RightContext->GetHash());
	if (bSuccess)
	{
		return true;
	}

	if (!HasExtendedTypeCheck() && RightContext->HasExtendedTypeCheck())
	{
		return RightContext->IsA(this);
	}

	return false;
}

/***********************************
 * Apply Meta
 ***********************************/

void PrintUnusedMetaValue(const FDataRawMetaItem* Item)
{
	if (!Item->Value.IsEmpty())
	{
		UE_LOG(LogDataReflection, Error, TEXT("      ? unused value \"%s\" for meta: \"%s\""), *ToFString(Item->Value), *ToFString(Item->Key));
	}
}

void PrintMissingMetaValue(const FDataRawMetaItem* Item)
{
	if (Item->Value.IsEmpty())
	{
		UE_LOG(LogDataReflection, Error, TEXT("      ? missing value for meta: \"%s\""), *ToFString(Item->Key));
	}
}

void PrintIrrelevantMeta(const FDataRawMetaItem* Item)
{
	UE_LOG(LogDataReflection, VeryVerbose, TEXT("    ? irrelevant meta: \"%s%s%s\""), *ToFString(Item->Key), Item->Value.IsEmpty() ? TEXT("") : TEXT(" = "), Item->Value.IsEmpty() ? *ToFString(Item->Value) : TEXT(""));
}

void PrintIrrelevantMeta(const FDataRawMeta& RawMeta)
{
	for (const auto& Item : RawMeta.Items)
	{
		PrintIrrelevantMeta(&Item);
	}
}

void PrintApplyMeta(const FDataRawMetaItem* Item)
{
	UE_LOG(LogDataReflection, VeryVerbose, TEXT("    + meta: \"%s%s%s\""), *ToFString(Item->Key), Item->Value.IsEmpty() ? TEXT("") : TEXT(" = "), Item->Value.IsEmpty() ? *ToFString(Item->Value) : TEXT(""));
}

void ApplyMetaItems(FDataField* Field, FDataRawMeta& RawMeta)
{
	if (const auto Strict = RawMeta.Find(FDataMetaType::Strict))
	{
		Field->Meta.bStrict = true;
		PrintUnusedMetaValue(Strict);
		PrintApplyMeta(Strict);

		RawMeta.Remove(FDataMetaType::Strict);
	}
	if (const auto Event = RawMeta.Find(FDataMetaType::Event))
	{
		Field->Meta.bEvent = true;
		PrintUnusedMetaValue(Event);
		PrintApplyMeta(Event);

		RawMeta.Remove(FDataMetaType::Event);
	}
	if (const auto Bubbles = RawMeta.Find(FDataMetaType::Bubbles))
	{
		Field->Meta.bBubbles = true;
		PrintUnusedMetaValue(Bubbles);
		PrintApplyMeta(Bubbles);

		RawMeta.Remove(FDataMetaType::Bubbles);
	}
	if (const auto Alias = RawMeta.Find(FDataMetaType::Alias))
	{
		Field->Meta.bAlias = !Alias->Value.IsEmpty();
		Field->Meta.Alias = ToFString(Alias->Value);
		PrintMissingMetaValue(Alias);
		PrintApplyMeta(Alias);

		RawMeta.Remove(FDataMetaType::Alias);
	}
	if (const auto Deprecated = RawMeta.Find(FDataMetaType::Deprecated))
	{
		Field->Meta.bDeprecated = true;
		PrintUnusedMetaValue(Deprecated);
		PrintApplyMeta(Deprecated);

		RawMeta.Remove(FDataMetaType::Deprecated);
	}
	if (const auto ReadOnly = RawMeta.Find(FDataMetaType::ReadOnly))
	{
		Field->Meta.bReadOnly = true;
		PrintUnusedMetaValue(ReadOnly);
		PrintApplyMeta(ReadOnly);

		RawMeta.Remove(FDataMetaType::ReadOnly);
	}
	if (const auto Hidden = RawMeta.Find(FDataMetaType::Hidden))
	{
		Field->Meta.bHidden = true;
		PrintUnusedMetaValue(Hidden);
		PrintApplyMeta(Hidden);

		RawMeta.Remove(FDataMetaType::Hidden);
	}

	if (Field->Meta.bStrict && Field->Meta.bEvent)
	{
		Field->Meta.bEvent = false;
		Field->Meta.bBubbles = false;
		UE_LOG(LogDataReflection, Error, TEXT("Property with strict meta can't broadcast event"))
	}
}

void ApplyMetaItems(FDataLink* Link, FDataRawMeta& RawMeta)
{
	if (const auto Nullable = RawMeta.Find(FDataMetaType::Nullable))
	{
		Link->Meta.bNullable = true;
		PrintUnusedMetaValue(Nullable);
		PrintApplyMeta(Nullable);

		RawMeta.Remove(FDataMetaType::Nullable);
	}
}

template <typename T>
void ApplyMeta(T* Meta, PsDataTools::FDataRawMeta& RawMeta)
{
	ApplyMetaItems(Meta, RawMeta);
	PrintIrrelevantMeta(RawMeta);
	RawMeta.Reset();
}

/***********************************
 * FDataField
 ***********************************/

FDataField::FDataField(const FString& InName, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InContext, PsDataTools::FDataRawMeta& RawMeta)
	: Name(InName)
	, Index(InIndex)
	, Hash(InHash)
	, Context(InContext)
{
	ApplyMeta<FDataField>(this, RawMeta);
	Meta.EventType = FString::Printf(TEXT("%sChanged"), *Name);
}

const FString& FDataField::GetChangedEventName() const
{
	return Meta.EventType;
}

const FString& FDataField::GetAliasName() const
{
	return Meta.bAlias ? Meta.Alias : Name;
}

const FString& FDataField::GetNameForSerialize() const
{
	return GetAliasName();
}

/***********************************
 * FDataLink
 ***********************************/

FDataLink::FDataLink(const FDataField* InField, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InReturnContext, FLinkPathFunction InPathFunction, bool bInAbstract, PsDataTools::FDataRawMeta& RawMeta)
	: Field(InField)
	, Index(InIndex)
	, Hash(InHash)
	, ReturnContext(InReturnContext)
	, PathFunction(InPathFunction)
	, bAbstract(bInAbstract)
{
	ApplyMeta<FDataLink>(this, RawMeta);
}
