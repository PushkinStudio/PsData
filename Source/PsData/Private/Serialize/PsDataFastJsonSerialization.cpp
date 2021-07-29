// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataFastJsonSerialization.h"

#include "PsData.h"

/***********************************
 * Utils
 ***********************************/

bool IsSpace(TCHAR Char)
{
	return Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r';
}

bool IsJsonToken(TCHAR Char)
{
	return Char == '[' || Char == '{' || Char == ']' || Char == '}' || Char == ',' || Char == ':';
}

bool IsQuote(TCHAR Char)
{
	return Char == '"' || Char == '\'';
}

int32 FindJsonToken(const TCHAR* String, int32 StartPosition, int32 EndPosition)
{
	bool bQuote = false;
	char QuoteType = '?';
	bool bNextEscaped = false;

	int32 Index = StartPosition;
	while (Index < EndPosition)
	{
		const int32 Pos = Index;
		const auto c = String[Pos];
		Index++;

		if (bNextEscaped)
		{
			bNextEscaped = false;
			continue;
		}
		else if (c == '\\')
		{
			bNextEscaped = true;
			continue;
		}

		if (IsQuote(c))
		{
			if (bQuote)
			{
				if (QuoteType == String[Pos])
				{
					bQuote = false;
					continue;
				}
			}
			else
			{
				bQuote = true;
				QuoteType = String[Pos];
				continue;
			}
		}

		if (!bQuote && IsJsonToken(c))
		{
			return Pos;
		}
	}

	return INDEX_NONE;
}

bool IsEmpty(const TCHAR* String, int32 StartPosition, int32 EndPosition)
{
	int32 Index = StartPosition;
	while (Index <= EndPosition)
	{
		const int32 Pos = Index;
		const auto c = String[Pos];
		Index++;

		if (!IsSpace(c))
		{
			return false;
		}
	}

	return true;
}

void Trim(const TCHAR* String, int32& StartPosition, int32& EndPosition)
{
	while (StartPosition <= EndPosition)
	{
		if (IsSpace(String[StartPosition]))
		{
			++StartPosition;
		}
		else
		{
			break;
		}
	}

	while (StartPosition <= EndPosition)
	{
		if (IsSpace(String[EndPosition]))
		{
			--EndPosition;
		}
		else
		{
			break;
		}
	}

	if (StartPosition != EndPosition)
	{
		const auto a = String[StartPosition];
		const auto b = String[EndPosition];
		if (a == b && IsQuote(a))
		{
			++StartPosition;
			--EndPosition;
		}
	}
}

static const char CharToEscape[] = {'\\', '\n', '\r', '\t', '"'};
static const char EscapedChars[] = {'\\', 'n', 'r', 't', '"'};
static const uint32 MaxSupportedEscapeChars = UE_ARRAY_COUNT(CharToEscape);

FString StringToJsonString(const TCHAR* String, int32 StartPosition, int32 Count)
{
	FString Result;
	Result.Reserve(Count);

	for (int32 i = StartPosition; i < StartPosition + Count; ++i)
	{
		auto const c = String[i];
		bool bAppended = false;
		for (int32 j = 0; j < MaxSupportedEscapeChars; ++j)
		{
			if (c == CharToEscape[j])
			{
				Result.AppendChar('\\');
				Result.AppendChar(EscapedChars[j]);
				bAppended = true;
				break;
			}
		}

		if (!bAppended)
		{
			Result.AppendChar(c);
		}
	}

	return Result;
}

FString JsonStringToString(const TCHAR* String, int32 StartPosition, int32 Count)
{
	FString Result;
	Result.Reserve(Count);

	bool bEscaped = false;
	for (int32 i = StartPosition; i < StartPosition + Count; ++i)
	{
		auto const c = String[i];
		if (!bEscaped && c == '\\')
		{
			bEscaped = true;
			continue;
		}

		bool bAppended = false;
		for (int32 j = 0; j < MaxSupportedEscapeChars; ++j)
		{
			if (bEscaped && c == EscapedChars[j])
			{
				Result.AppendChar(CharToEscape[j]);
				bAppended = true;
				break;
			}
		}

		if (!bAppended)
		{
			if (bEscaped)
			{
				Result.AppendChar('\\');
				Result.AppendChar(c);
			}
			else
			{
				Result.AppendChar(c);
			}
		}

		bEscaped = false;
	}

	return Result;
}

/***********************************
 * FPsDataFastJsonSerializer
 ***********************************/

FPsDataFastJsonSerializer::FPsDataFastJsonSerializer(bool bInPretty)
	: bPretty(bInPretty)
	, Depth(0)
{
}

void FPsDataFastJsonSerializer::AppendComma()
{
	if (JsonString.Len() > 0 && JsonString[JsonString.Len() - 1] == ':')
	{
		return;
	}

	if (CommaHelper.Contains(Depth))
	{
		JsonString.AppendChar(',');
	}
	else
	{
		CommaHelper.Add(Depth);
	}
}

void FPsDataFastJsonSerializer::AppendSpace()
{
	if (bPretty)
	{
		JsonString.AppendChar('\n');
		for (int32 i = 0; i < Depth; ++i)
		{
			JsonString.AppendChar('\t');
		}
	}
}

void FPsDataFastJsonSerializer::AppendValueSpace()
{
	if (bPretty)
	{
		JsonString.AppendChar(' ');
	}
}

void FPsDataFastJsonSerializer::WriteKey(const FString& Key)
{
	AppendComma();
	AppendSpace();

	JsonString.AppendChar('"');
	JsonString.Append(StringToJsonString(Key.GetCharArray().GetData(), 0, Key.Len()));
	JsonString.AppendChar('"');
	JsonString.AppendChar(':');
}

void FPsDataFastJsonSerializer::WriteArray()
{
	AppendComma();
	AppendValueSpace();

	++Depth;
	JsonString.AppendChar('[');
}

void FPsDataFastJsonSerializer::WriteObject()
{
	AppendComma();
	AppendValueSpace();

	++Depth;
	JsonString.AppendChar('{');
}

void FPsDataFastJsonSerializer::WriteValue(int32 Value)
{
	AppendComma();
	AppendValueSpace();

	JsonString.Appendf(TEXT("%d"), Value);
}

void FPsDataFastJsonSerializer::WriteValue(int64 Value)
{
	AppendComma();
	AppendValueSpace();

	JsonString.Appendf(TEXT("%lld"), Value);
}

void FPsDataFastJsonSerializer::WriteValue(uint8 Value)
{
	AppendComma();
	AppendValueSpace();

	JsonString.Appendf(TEXT("%d"), Value);
}

void FPsDataFastJsonSerializer::WriteValue(float Value)
{
	AppendComma();
	AppendValueSpace();

	JsonString.Appendf(TEXT("%f"), Value);
}

void FPsDataFastJsonSerializer::WriteValue(bool Value)
{
	AppendComma();
	AppendValueSpace();

	JsonString.Append(Value ? "true" : "false");
}

void FPsDataFastJsonSerializer::WriteValue(const FString& Value)
{
	AppendComma();
	AppendValueSpace();

	JsonString.AppendChar('"');
	JsonString.Append(StringToJsonString(Value.GetCharArray().GetData(), 0, Value.Len()));
	JsonString.AppendChar('"');
}

void FPsDataFastJsonSerializer::WriteValue(const FName& Value)
{
	WriteValue(Value.ToString());
}

void FPsDataFastJsonSerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		AppendComma();
		AppendValueSpace();

		JsonString.Append("null");
	}
	else
	{
		WriteObject();
		PsDataTools::FPsDataFriend::Serialize(Value, this);
		PopObject();
	}
}

void FPsDataFastJsonSerializer::PopKey(const FString& Key)
{
}

void FPsDataFastJsonSerializer::PopArray()
{
	CommaHelper.Remove(Depth);
	--Depth;

	const auto Last = JsonString[JsonString.Len() - 1];
	if (Last != '[')
	{
		AppendSpace();
	}

	JsonString.AppendChar(']');
}

void FPsDataFastJsonSerializer::PopObject()
{
	CommaHelper.Remove(Depth);
	--Depth;

	const auto Last = JsonString[JsonString.Len() - 1];
	if (Last != '{')
	{
		AppendSpace();
	}

	JsonString.AppendChar('}');
}

/***********************************
 * FPsDataFastJsonLink
 ***********************************/

FPsDataFastJsonPointer::FPsDataFastJsonPointer(EPsDataFastJsonToken InToken, int32 InStartPosition, int32 InEndPosition, int32 InDepth)
	: Token(InToken)
	, Depth(InDepth)
	, StartPosition(InStartPosition)
	, EndPosition(InEndPosition)
	, bHasString(false)
{
}

const FString& FPsDataFastJsonPointer::GetString(const TCHAR* Source)
{
	if (!bHasString)
	{
		bHasString = true;
		if (!IsEmpty(Source, StartPosition, EndPosition))
		{
			Trim(Source, StartPosition, EndPosition);
			String = JsonStringToString(Source, StartPosition, EndPosition - StartPosition + 1);
		}
	}

	return String;
}

void FPsDataFastJsonPointer::Reset()
{
	if (bHasString)
	{
		bHasString = false;
		String.Empty();
	}
}

/***********************************
 * FPsDataFastJsonDeserializer
 ***********************************/

FPsDataFastJsonDeserializer::FPsDataFastJsonDeserializer(const FString& InJsonString)
	: FPsDataDeserializer()
	, Source(InJsonString.GetCharArray().GetData())
	, Size(InJsonString.Len())
	, PointerIndex(0)
{
	Pointers.Reserve(100);
	Parse();

	DepthStack.Reserve(10);
}

void FPsDataFastJsonDeserializer::Parse()
{
	int32 PrevIndex = -1;
	int32 Index = -1;
	int32 Depth = 0;

	while (true)
	{
		PrevIndex = Index + 1;
		Index = FindJsonToken(Source, PrevIndex, Size);
		if (Index == INDEX_NONE)
		{
			break;
		}

		const auto c = Source[Index];

		if ((c == '}' || c == ']' || c == ',') && !IsEmpty(Source, PrevIndex, Index - 1))
		{
			Pointers.Add({EPsDataFastJsonToken::Value, PrevIndex, Index - 1, Depth + 1});
		}

		switch (c)
		{
		case '{':
			Pointers.Add({EPsDataFastJsonToken::OpenObject, Index, Index, ++Depth});
			break;
		case '}':
			Pointers.Add({EPsDataFastJsonToken::CloseObject, Index, Index, Depth--});
			break;
		case '[':
			Pointers.Add({EPsDataFastJsonToken::OpenArray, Index, Index, ++Depth});
			break;
		case ']':
			Pointers.Add({EPsDataFastJsonToken::CloseArray, Index, Index, Depth--});
			break;
		case ':':
			Pointers.Add({EPsDataFastJsonToken::Key, PrevIndex, Index - 1, Depth});
			break;
		case ',':
			Pointers.Add({EPsDataFastJsonToken::Comma, Index, Index, Depth});
			break;
		}
	}
}

void FPsDataFastJsonDeserializer::SkipComma()
{
	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token == EPsDataFastJsonToken::Comma)
	{
		++PointerIndex;
	}
}

bool FPsDataFastJsonDeserializer::ReadKey(FString& OutKey)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Key)
	{
		return false;
	}

	OutKey = Pointer.GetString(Source);
	DepthStack.Push(Pointer.Depth);
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadArray()
{
	SkipComma();

	const auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::OpenArray)
	{
		return false;
	}

	DepthStack.Push(Pointer.Depth);

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadIndex()
{
	const auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token == EPsDataFastJsonToken::CloseArray)
	{
		return false;
	}
	return true;
}

bool FPsDataFastJsonDeserializer::ReadObject()
{
	SkipComma();

	const auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::OpenObject)
	{
		return false;
	}

	DepthStack.Push(Pointer.Depth);

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(int32& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	const FString& Value = Pointer.GetString(Source);
	if (!Value.IsNumeric())
	{
		return false;
	}

	OutValue = FCString::Atoi(*Value);
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(int64& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	const FString& Value = Pointer.GetString(Source);
	if (!Value.IsNumeric())
	{
		return false;
	}

	OutValue = FCString::Atoi64(*Value);
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(uint8& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	const FString& Value = Pointer.GetString(Source);
	if (!Value.IsNumeric())
	{
		return false;
	}

	OutValue = FCString::Atoi(*Value);
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(float& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	const FString& Value = Pointer.GetString(Source);
	if (!Value.IsNumeric())
	{
		return false;
	}

	OutValue = FCString::Atof(*Value);
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(bool& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	const FString& Value = Pointer.GetString(Source);
	if (Value.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		OutValue = true;
	}
	else if (Value.Equals(TEXT("false"), ESearchCase::IgnoreCase))
	{
		OutValue = false;
	}
	else if (Value.IsNumeric())
	{
		OutValue = FCString::Atoi(*Value) ? true : false;
	}
	else
	{
		return false;
	}

	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(FString& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	OutValue = Pointer.GetString(Source);
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(FName& OutValue)
{
	FString Out;
	const bool bResult = ReadValue(Out);
	OutValue = bResult ? FName(*Out) : NAME_None;
	return bResult;
}

bool FPsDataFastJsonDeserializer::ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token == EPsDataFastJsonToken::Value)
	{
		const auto Value = Pointer.GetString(Source);
		if (Value.Equals(TEXT("null"), ESearchCase::IgnoreCase))
		{
			OutValue = nullptr;
			Pointer.Reset();

			++PointerIndex;
			return true;
		}
	}
	else if (ReadObject())
	{
		if (OutValue == nullptr)
		{
			OutValue = Allocator();
		}

		PsDataTools::FPsDataFriend::Deserialize(OutValue, this);

		PopObject();

		return true;
	}

	return false;
}

void FPsDataFastJsonDeserializer::PopKey(const FString& Key)
{
	const int32 Depth = DepthStack.Pop(false);
	for (int32 i = PointerIndex; i < Pointers.Num(); ++i)
	{
		const auto& Pointer = Pointers[i];
		if (Pointer.Depth == Depth)
		{
			PointerIndex = i;
			return;
		}
	}

	checkNoEntry();
}

void FPsDataFastJsonDeserializer::PopIndex()
{
}

void FPsDataFastJsonDeserializer::PopArray()
{
	const int32 Depth = DepthStack.Pop(false);
	for (int32 i = PointerIndex; i < Pointers.Num(); ++i)
	{
		const auto& Pointer = Pointers[i];
		if (Pointer.Token == EPsDataFastJsonToken::CloseArray && Pointer.Depth == Depth)
		{
			PointerIndex = i + 1;
			return;
		}
	}

	checkNoEntry();
}

void FPsDataFastJsonDeserializer::PopObject()
{
	const int32 Depth = DepthStack.Pop(false);
	for (int32 i = PointerIndex; i < Pointers.Num(); ++i)
	{
		const auto& Pointer = Pointers[i];
		if (Pointer.Token == EPsDataFastJsonToken::CloseObject && Pointer.Depth == Depth)
		{
			PointerIndex = i + 1;
			return;
		}
	}

	checkNoEntry();
}
