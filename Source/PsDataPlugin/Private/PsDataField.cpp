// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsDataField.h"

#include "PsData.h"
#include "PsDataCore.h"

/***********************************
 * EDataMetaType
 ***********************************/

const char* EDataMetaType::Strict = "strict";
const char* EDataMetaType::Event = "event";
const char* EDataMetaType::Bubbles = "bubbles";
const char* EDataMetaType::Alias = "alias";
const char* EDataMetaType::ReadOnly = "readonly";
const char* EDataMetaType::Deprecated = "deprecated";
const char* EDataMetaType::Nullable = "nullable";

/***********************************
 * FDataFieldMeta
 ***********************************/

FDataFieldMeta::FDataFieldMeta()
	: bStrict(false)
	, bEvent(false)
	, bBubbles(false)
	, bDeprecated(false)
	, bReadOnly(false)
	, Alias()
	, EventType()
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

FDataFieldFunctions::FDataFieldFunctions(UFunction* InGetFunction, UFunction* InSetFunction)
	: GetFunction(InGetFunction)
	, SetFunction(InSetFunction)
{
	check(InGetFunction);
	check(InSetFunction);
}

/***********************************
 * FAbstractDataTypeContext
 ***********************************/

UField* FAbstractDataTypeContext::GetUE4Type() const
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
 * Meta prepare
 ***********************************/

struct FMetaToken
{
	char c;
	int32 Size;
	const char* String;

	FMetaToken(const char* InString, int32 InSize)
		: c(0)
		, Size(InSize)
		, String(InString)
	{
	}

	FMetaToken(char InC)
		: c(InC)
		, Size(0)
		, String(nullptr)
	{
	}
};

char ToLowerCase(char c)
{
	if (c >= 'A' && c <= 'Z')
	{
		const char o = 'Z' - 'z';
		return c - o;
	}
	return c;
}

bool Equal(const char* str1, const char* str2)
{
	while (*str1 != 0 && *str2 != 0)
	{
		if (ToLowerCase(*str1) != ToLowerCase(*str2))
		{
			return false;
		}

		str1 += 1;
		str2 += 1;
	}

	return true;
}

std::pair<int, int> Trim(const char* Str, int Size)
{
	if (Size == 0)
	{
		return std::pair<int, int>(0, 0);
	}
	int32 i = 0;
	while (Str[i] == ' ')
	{
		++i;
		--Size;
	}
	while (Size > 0 && Str[i + Size - 1] == ' ')
	{
		--Size;
	}

	return std::pair<int, int>(i, Size);
}

void AddToken(TArray<FMetaToken>& Tokens, const char*& Str, int& Size, char c)
{
	std::pair<int, int> range = Trim(Str, Size);
	if (range.second > 0)
	{
		Tokens.Add(FMetaToken(&Str[range.first], range.second));
	}
	Tokens.Add(FMetaToken(c));
	Str = Str + (Size + 1);
	Size = 0;
}

void AddToken(TArray<FMetaToken>& Tokens, const char*& Str, int& Size)
{
	std::pair<int, int> range = Trim(Str, Size);
	if (range.second > 0)
	{
		Tokens.Add(FMetaToken(&Str[range.first], range.second));
	}
	Str = Str + (Size + 1);
	Size = 0;
}

bool CheckTokens(TArray<FMetaToken> Tokens, int32 Index, const char* Pattern)
{
	while (*Pattern != 0)
	{
		if (*Pattern == '%')
		{
			if (Tokens[Index].String == nullptr)
			{
				return false;
			}
		}
		else
		{
			if (Tokens[Index].c != *Pattern)
			{
				return false;
			}
		}
		Pattern += 1;
		++Index;
	}
	return true;
}

void ParseMetaPair(FDataField* Field, const char* Key, int32 KeySize, const char* Value, int32 ValueSize)
{
	bool bError = false;
	if (Equal(Key, EDataMetaType::Strict))
	{
		ensureMsgf(Value == nullptr, TEXT("Unused value!"));
		Field->Meta.bStrict = true;
	}
	else if (Equal(Key, EDataMetaType::Event))
	{
		Field->Meta.bEvent = true;
		Field->Meta.EventType = Value ? FString(ValueSize, Value) : FString::Printf(TEXT("%sChanged"), *Field->Name);
	}
	else if (Equal(Key, EDataMetaType::Bubbles))
	{
		ensureMsgf(Value == nullptr, TEXT("Unused value!"));
		Field->Meta.bBubbles = true;
	}
	else if (Equal(Key, EDataMetaType::Alias))
	{
		checkf(Value != nullptr, TEXT("Value needed!"));
		Field->Meta.Alias = Value ? FString(ValueSize, Value) : TEXT("");
	}
	else if (Equal(Key, EDataMetaType::Deprecated))
	{
		ensureMsgf(Value == nullptr, TEXT("Unused value!"));
		Field->Meta.bDeprecated = true;
	}
	else if (Equal(Key, EDataMetaType::ReadOnly))
	{
		ensureMsgf(Value == nullptr, TEXT("Unused value!"));
		Field->Meta.bReadOnly = true;
	}
	else
	{
		bError = true;
	}

	if (bError)
	{
		UE_LOG(LogData, Error, TEXT("      ? unknown meta: \"%s%s%s\""), *FString(KeySize, Key), Value ? TEXT(" = ") : TEXT(""), Value ? *FString(ValueSize, Value) : TEXT(""));
	}
	else
	{
		UE_LOG(LogData, Verbose, TEXT("    + meta: \"%s%s%s\""), *FString(KeySize, Key), Value ? TEXT(" = ") : TEXT(""), Value ? *FString(ValueSize, Value) : TEXT(""));
	}

	if (Field->Meta.bStrict && Field->Meta.bEvent)
	{
		Field->Meta.bEvent = false;
		Field->Meta.bBubbles = false;
		Field->Meta.EventType = TEXT("");
		UE_LOG(LogData, Error, TEXT("Property with strict meta can't broadcast event"))
	}
}

void ParseMetaPair(FDataLink* Link, const char* Key, int32 KeySize, const char* Value, int32 ValueSize)
{
	bool bError = false;
	if (Equal(Key, EDataMetaType::Nullable))
	{
		ensureMsgf(Value == nullptr, TEXT("Unused value!"));
		Link->Meta.bNullable = true;
	}
	else
	{
		bError = true;
	}

	if (bError)
	{
		UE_LOG(LogData, Error, TEXT("      ? unknown meta: \"%s%s%s\""), *FString(KeySize, Key), Value ? TEXT(" = ") : TEXT(""), Value ? *FString(ValueSize, Value) : TEXT(""));
	}
	else
	{
		UE_LOG(LogData, Verbose, TEXT("    + meta: \"%s%s%s\""), *FString(KeySize, Key), Value ? TEXT(" = ") : TEXT(""), Value ? *FString(ValueSize, Value) : TEXT(""));
	}
}

template <typename T>
void ParseMeta(T* Meta, const TArray<const char*>& Collection)
{
	TArray<FMetaToken> Tokens;
	Tokens.Reserve(Collection.Num() * 5);
	for (const char* Str : Collection)
	{
		char Quote = 0;

		int32 i = 0;
		while (Str[i] != 0)
		{
			const char c = Str[i];
			if (c == '\'' || c == '\"')
			{
				if (Quote == 0)
				{
					Quote = c;
					AddToken(Tokens, Str, i);
					continue;
				}
				else if (Quote == c)
				{
					Quote = 0;
					AddToken(Tokens, Str, i);
					continue;
				}
			}

			if (Quote == 0 && (c == ',' || c == '='))
			{
				AddToken(Tokens, Str, i, c);
				continue;
			}

			++i;
		}
		if (i > 0)
		{
			AddToken(Tokens, Str, i);
			Tokens.Add(FMetaToken(','));
		}
	}

	int32 TokenOffset = 0;
	while (Tokens.Num() > TokenOffset)
	{
		if (CheckTokens(Tokens, TokenOffset, "%,"))
		{
			ParseMetaPair(Meta, Tokens[TokenOffset].String, Tokens[TokenOffset].Size, nullptr, 0);
			TokenOffset += 2;
		}
		else if (CheckTokens(Tokens, TokenOffset, "%=%,"))
		{
			ParseMetaPair(Meta, Tokens[TokenOffset].String, Tokens[TokenOffset].Size, Tokens[TokenOffset + 2].String, Tokens[TokenOffset + 2].Size);
			TokenOffset += 4;
		}
		else
		{
			UE_LOG(LogData, Error, TEXT("      ? unknown meta pattern: \"%s\""), Tokens[TokenOffset].String ? *FString(Tokens[TokenOffset].Size, Tokens[TokenOffset].String) : *FString(1, &Tokens[TokenOffset].c));
			TokenOffset += 1;
		}
	}
}

/***********************************
 * FDataField
 ***********************************/

FDataField::FDataField(const FString& InName, int32 InIndex, int32 InHash, FAbstractDataTypeContext* InContext, const TArray<const char*>& MetaCollection)
	: Name(InName)
	, Index(InIndex)
	, Hash(InHash)
	, Context(InContext)
{
	ParseMeta<FDataField>(this, MetaCollection);
}

const FString& FDataField::GetChangedEventName() const
{
	return Meta.EventType;
}

/***********************************
 * FDataLink
 ***********************************/

FDataLink::FDataLink(const FString& InName, const FString& InPath, bool bInPathProperty, const FString& InReturnType, int32 InHash, bool bInAbstract, bool bInCollection, const TArray<const char*>& MetaCollection)
	: Name(InName)
	, Path(InPath)
	, bPathProperty(bInPathProperty)
	, ReturnType(InReturnType)
	, Hash(InHash)
	, bCollection(bInCollection)
	, bAbstract(bInAbstract)
{
	ParseMeta<FDataLink>(this, MetaCollection);
}
