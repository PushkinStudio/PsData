// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataTraits.h"

#include "CoreMinimal.h"

DEFINE_LOG_CATEGORY_STATIC(LogDataReflection, VeryVerbose, All);

namespace PsDataTools
{

/***********************************
 * FDataMetaType
 ***********************************/

struct PSDATA_API FDataMetaType
{
	static const FDataStringViewChar Strict;
	static const FDataStringViewChar Event;
	static const FDataStringViewChar Bubbles;
	static const FDataStringViewChar Alias;
	static const FDataStringViewChar ReadOnly;
	static const FDataStringViewChar Deprecated;
	static const FDataStringViewChar Nullable;
	static const FDataStringViewChar Hidden;
	static const FDataStringViewChar CustomType;
};

/***********************************
 * FDataRawMetaItem
 ***********************************/

struct PSDATA_API FDataRawMetaItem
{
	FDataStringViewChar Key;
	FDataStringViewChar Value;

	FDataRawMetaItem(const FDataStringViewChar& InKey, const FDataStringViewChar& InValue)
		: Key(InKey)
		, Value(InValue)
	{
	}
};

/***********************************
 * FDataRawMeta
 ***********************************/

struct PSDATA_API FDataRawMeta
{
	FDataRawMeta();

	void Append(const char* MetaString);
	void Append(const FDataRawMeta& OtherMeta);
	void Reset();
	FDataRawMetaItem* Find(const FDataStringViewChar& Key);
	const FDataRawMetaItem* Find(const FDataStringViewChar& Key) const;
	bool Contains(const FDataStringViewChar& Key) const;
	bool Remove(const FDataStringViewChar& Key);

	TArray<FDataRawMetaItem> Items;
};

} // namespace PsDataTools

/***********************************
 * FDataFieldMeta
 ***********************************/

struct PSDATA_API FDataFieldMeta
{
	bool bStrict;
	bool bEvent;
	bool bBubbles;
	bool bDeprecated;
	bool bReadOnly;
	bool bAlias;
	bool bDefault;
	bool bHidden;
	bool bCustomType;
	FString Alias;
	FString EventType;

	FDataFieldMeta();
};

/***********************************
 * FDataLinkMeta
 ***********************************/

struct PSDATA_API FDataLinkMeta
{
	bool bNullable;

	FDataLinkMeta();
};

/***********************************
 * FDataFieldFunctions
 ***********************************/

enum class EDataFieldType : uint8
{
	VALUE = 0,
	ARRAY = 1,
	MAP = 2
};

struct PSDATA_API FDataFieldFunctions
{
	UClass* Class;
	FName GetFunctionName;
	FName SetFunctionName;

	FDataFieldFunctions(UClass* Class, FName GetFunctionName, FName SetFunctionName);
	FDataFieldFunctions(UClass* Class, EDataFieldType FieldType);

	UFunction* ResolveGetFunction() const;
	UFunction* ResolveSetFunction() const;
};

struct PSDATA_API FDataLinkFunctions
{
	UClass* Class;
	FName FunctionName;

	FDataLinkFunctions(UClass* Class, FName FunctionName);
	FDataLinkFunctions(UClass* Class, EDataFieldType FieldType);

	UFunction* ResolveFunction() const;
};

/***********************************
 * Field Context
 ***********************************/

struct PSDATA_API FAbstractDataTypeContext
{
	virtual ~FAbstractDataTypeContext() {}

	virtual FString GetCppType() const = 0;
	virtual FString GetCppContentType() const = 0;
	virtual uint32 GetHash() const = 0;
	virtual FDataFieldFunctions GetUFunctions() const = 0;
	virtual FDataLinkFunctions GetLinkUFunctions() const = 0;
	virtual UField* GetUEType() const;

	virtual bool IsArray() const;
	virtual bool IsMap() const;
	virtual bool IsContainer() const;
	virtual bool IsData() const;
	virtual bool IsEnum() const;

	virtual bool HasExtendedTypeCheck() const;
	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const;
};

/***********************************
 * FDataField
 ***********************************/

struct PSDATA_API FDataField
{
	FString Name;
	int32 Index;
	int32 Hash;
	FAbstractDataTypeContext* Context;
	FDataFieldMeta Meta;

	FDataField(const FString& InName, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InContext, PsDataTools::FDataRawMeta& RawMeta);
	const FString& GetChangedEventName() const;
	const FString& GetAliasName() const;
	const FString& GetNameForSerialize() const;
};

/***********************************
 * FDataLink
 ***********************************/

using FLinkPathFunction = TFunction<void(class UPsData* Data, FString& OutPath)>;

struct PSDATA_API FDataLink
{
	const FDataField* Field;
	int32 Index;
	int32 Hash;
	FAbstractDataTypeContext* ReturnContext;
	FLinkPathFunction PathFunction;
	bool bAbstract;
	FDataLinkMeta Meta;

	FDataLink(const FDataField* InField, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InReturnContext, FLinkPathFunction InPathFunction, bool bInAbstract, PsDataTools::FDataRawMeta& RawMeta);
};
