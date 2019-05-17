// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataTraits.h"

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"

struct FAbstractDataMemory;

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

struct PSDATAPLUGIN_API FDataFieldFunctions
{
	UFunction* GetFunction;
	UFunction* SetFunction;
	FDataFieldFunctions(UFunction* InGetFunction, UFunction* InSetFunction);
};

/***********************************
 * Field Context
 ***********************************/

struct PSDATAPLUGIN_API FAbstractDataTypeContext
{
	virtual const FString& GetCppType() const = 0;
	virtual const FString& GetCppContentType() const = 0;
	virtual uint32 GetHash() const = 0;
	virtual TSharedPtr<const FDataFieldFunctions> GetUFunctions() const = 0;
	virtual FAbstractDataMemory* AllocateMemory() const = 0;
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

struct PSDATAPLUGIN_API FDataField
{
public:
	FString Name;
	int32 Index;
	int32 Hash;
	FAbstractDataTypeContext* Context;
	FDataFieldMeta Meta;

	FDataField(const FString& InName, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InContext, const TArray<const char*>& MetaCollection);
	const FString& GetChangeEventName() const;
};

/***********************************
 * FDataLink
 ***********************************/

struct PSDATAPLUGIN_API FDataLink
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
