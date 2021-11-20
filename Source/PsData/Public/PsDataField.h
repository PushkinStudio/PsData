// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataTraits.h"

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"

struct FAbstractDataProperty;

/***********************************
 * EDataMetaType
 ***********************************/

struct EDataMetaType
{
	static const char* Strict;
	static const char* Event;
	static const char* Bubbles;
	static const char* Alias;
	static const char* ReadOnly;
	static const char* Deprecated;
	static const char* Nullable;
};

/***********************************
 * FDataFieldMeta
 ***********************************/

struct FDataFieldMeta
{
	bool bStrict;
	bool bEvent;
	bool bBubbles;
	bool bDeprecated;
	bool bReadOnly;
	bool bAlias;
	bool bDefault;
	FString Alias;
	FString EventType;

	FDataFieldMeta();
};

/***********************************
 * FDataLinkMeta
 ***********************************/

struct FDataLinkMeta
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

/***********************************
 * Field Context
 ***********************************/

struct PSDATA_API FAbstractDataTypeContext
{
	virtual FString GetCppType() const = 0;
	virtual FString GetCppContentType() const = 0;
	virtual uint32 GetHash() const = 0;
	virtual FDataFieldFunctions GetUFunctions() const = 0;
	virtual UField* GetUE4Type() const;

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
public:
	FString Name;
	int32 Index;
	int32 Hash;
	FAbstractDataTypeContext* Context;
	FDataFieldMeta Meta;

	FDataField(const FString& InName, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InContext, const TArray<const char*>& MetaCollection);
	const FString& GetChangedEventName() const;
	const FString& GetAliasName() const;
	const FString& GetNameForSerialize() const;
};

/***********************************
 * FDataLink
 ***********************************/

struct PSDATA_API FDataLink
{
public:
	FString Name;
	FString Path;
	bool bPathProperty;
	FString ReturnType;
	int32 Hash;
	bool bCollection;
	bool bAbstract;
	FDataLinkMeta Meta;

	FDataLink(const FString& InName, const FString& InPath, bool bInPathProperty, const FString& InReturnType, int32 InHash, bool bInAbstract, bool bInCollection, const TArray<const char*>& MetaCollection);
};
