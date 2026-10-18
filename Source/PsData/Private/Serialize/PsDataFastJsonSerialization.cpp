// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Serialize/PsDataFastJsonSerialization.h"

#include "PsData.h"
#include "PsDataDefines.h"
#include "PsDataUtils.h"

#include <cmath>

/***********************************
 * Utils
 ***********************************/

namespace PsDataTools
{
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

void Trim(const TCHAR* String, int32& StartPosition, int32& EndPosition, bool& bOutQuoted)
{
	bOutQuoted = false;

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
			bOutQuoted = true;
		}
	}
}

// RFC 8259 section 7: quote and backslash, the short forms, and \u00XX for the remaining control characters
void AppendStringAsJsonString(TArray<TCHAR>& Json, const TCHAR* String, int32 StartPosition, int32 Count)
{
	static constexpr TCHAR HexDigits[] = TEXT("0123456789abcdef");

	Json.Reserve(Json.Num() + 2 + Count + Count / 8);
	Json.Add('"');

	for (int32 i = StartPosition; i < StartPosition + Count; ++i)
	{
		const TCHAR c = String[i];
		switch (c)
		{
		case '"': Json.Append(TEXT("\\\""), 2); break;
		case '\\': Json.Append(TEXT("\\\\"), 2); break;
		case '\n': Json.Append(TEXT("\\n"), 2); break;
		case '\r': Json.Append(TEXT("\\r"), 2); break;
		case '\t': Json.Append(TEXT("\\t"), 2); break;
		case '\b': Json.Append(TEXT("\\b"), 2); break;
		case '\f': Json.Append(TEXT("\\f"), 2); break;
		default:
			if (c < 0x20)
			{
				Json.Append(TEXT("\\u00"), 4);
				Json.Add(HexDigits[(c >> 4) & 0xF]);
				Json.Add(HexDigits[c & 0xF]);
			}
			else
			{
				Json.Add(c);
			}
		}
	}

	Json.Add('"');
}

int32 HexToInt(TCHAR Char)
{
	if (Char >= '0' && Char <= '9')
	{
		return Char - '0';
	}
	if (Char >= 'a' && Char <= 'f')
	{
		return Char - 'a' + 10;
	}
	if (Char >= 'A' && Char <= 'F')
	{
		return Char - 'A' + 10;
	}
	return INDEX_NONE;
}

FString JsonStringToString(const TCHAR* String, int32 StartPosition, int32 Count)
{
	FString Result;
	Result.Reserve(Count);

	const int32 EndPosition = StartPosition + Count;
	for (int32 i = StartPosition; i < EndPosition; ++i)
	{
		const TCHAR c = String[i];
		if (c != '\\' || i + 1 >= EndPosition)
		{
			Result.AppendChar(c);
			continue;
		}

		const TCHAR Escaped = String[++i];
		switch (Escaped)
		{
		case '"':
		case '\'':
		case '\\':
		case '/': Result.AppendChar(Escaped); break;
		case 'n': Result.AppendChar('\n'); break;
		case 'r': Result.AppendChar('\r'); break;
		case 't': Result.AppendChar('\t'); break;
		case 'b': Result.AppendChar('\b'); break;
		case 'f': Result.AppendChar('\f'); break;
		case 'u':
		{
			// One UTF-16 code unit; TCHAR is UTF-16, so a surrogate pair written as two escapes reassembles itself
			int32 Code = 0;
			bool bValid = i + 4 < EndPosition;
			for (int32 Digit = 1; bValid && Digit <= 4; ++Digit)
			{
				const int32 Value = HexToInt(String[i + Digit]);
				bValid = Value != INDEX_NONE;
				Code = Code * 16 + Value;
			}

			if (bValid)
			{
				Result.AppendChar(static_cast<TCHAR>(Code));
				i += 4;
			}
			else
			{
				Result.AppendChar('\\');
				Result.AppendChar(Escaped);
			}
			break;
		}
		default:
			// Not an escape JSON defines: kept as written
			Result.AppendChar('\\');
			Result.AppendChar(Escaped);
		}
	}

	return Result;
}

} // namespace PsDataTools

using namespace PsDataTools;

/***********************************
 * FPsDataFastJsonSerializer
 ***********************************/

FPsDataFastJsonSerializer::FPsDataFastJsonSerializer(bool bInPretty, int32 BufferSize)
	: bPretty(bInPretty)
	, Depth(0)
{
	Buffer.Reserve(BufferSize / sizeof(TCHAR));
}

FString& FPsDataFastJsonSerializer::GetJsonString()
{
	if (JsonString.IsEmpty() && Buffer.Num() > 0)
	{
		auto& CharArray = JsonString.GetCharArray();
		CharArray = std::move(Buffer);
		CharArray.Add('\0');
	}

	return JsonString;
}

void FPsDataFastJsonSerializer::AppendComma()
{
	if (Buffer.Num() > 0 && Buffer.Last() == ':')
	{
		return;
	}

	if (CommaHelper.Contains(Depth))
	{
		Buffer.Add(',');
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
		if (Buffer.Num() == 0)
		{
			return;
		}

		if (Buffer.Last() == ':')
		{
			Buffer.Add(' ');
			return;
		}

		Buffer.Add('\n');
		for (int32 i = 0; i < Depth; ++i)
		{
			Buffer.Add('\t');
		}
	}
}

void FPsDataFastJsonSerializer::AppendValueSpace()
{
	if (bPretty)
	{
		Buffer.Add(' ');
	}
}

void FPsDataFastJsonSerializer::WriteKey(const FString& Key)
{
	AppendComma();
	AppendSpace();

	// Escaped: Translate passes keys through from arbitrary JSON, not only property names
	AppendStringAsJsonString(Buffer, Key.GetCharArray().GetData(), 0, Key.Len());
	Buffer.Add(':');
}

void FPsDataFastJsonSerializer::WriteArray()
{
	AppendComma();
	AppendSpace();

	++Depth;
	Buffer.Add('[');
}

void FPsDataFastJsonSerializer::WriteObject()
{
	AppendComma();
	AppendSpace();

	++Depth;
	Buffer.Add('{');
}

void FPsDataFastJsonSerializer::WriteValue(int32 Value)
{
	AppendComma();
	AppendSpace();

	PsDataTools::Numbers::ToString(Value, Buffer);
}

void FPsDataFastJsonSerializer::WriteValue(int64 Value)
{
	AppendComma();
	AppendSpace();

	PsDataTools::Numbers::ToString(Value, Buffer);
}

void FPsDataFastJsonSerializer::WriteValue(uint8 Value)
{
	AppendComma();
	AppendSpace();

	PsDataTools::Numbers::ToString(Value, Buffer);
}

void FPsDataFastJsonSerializer::WriteValue(float Value)
{
	AppendComma();
	AppendSpace();

	PsDataTools::Numbers::ToString(Value, Buffer);
}

void FPsDataFastJsonSerializer::WriteValue(double Value)
{
	AppendComma();
	AppendSpace();

	if (!FMath::IsFinite(Value))
	{
		Buffer.Append(TEXT("null"), 4); // ECMA-404.8
		return;
	}

	// The shorter of 15 and 17 significant digits that reads back as the same double
	TCHAR String[32];
	int32 Length = FCString::Snprintf(String, UE_ARRAY_COUNT(String), TEXT("%.15g"), Value);
	if (FCString::Atod(String) != Value)
	{
		Length = FCString::Snprintf(String, UE_ARRAY_COUNT(String), TEXT("%.17g"), Value);
	}
	Buffer.Append(String, Length);
}

void FPsDataFastJsonSerializer::WriteValue(bool Value)
{
	AppendComma();
	AppendSpace();

	if (Value)
	{
		Buffer.Append(TEXT("true"), 4);
	}
	else
	{
		Buffer.Append(TEXT("false"), 5);
	}
}

void FPsDataFastJsonSerializer::WriteValue(const FString& Value)
{
	AppendComma();
	AppendSpace();

	AppendStringAsJsonString(Buffer, Value.GetCharArray().GetData(), 0, Value.Len());
}

void FPsDataFastJsonSerializer::WriteValue(const FName& Value)
{
	FString StringValue = Value.ToString();
	StringValue.ToLowerInline();
	WriteValue(StringValue);
}

void FPsDataFastJsonSerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		AppendComma();
		AppendSpace();

		Buffer.Append(TEXT("null"), 4);
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

	if (bPretty && Buffer.Last() != '[')
	{
		AppendSpace();
	}

	Buffer.Add(']');
}

void FPsDataFastJsonSerializer::PopObject()
{
	CommaHelper.Remove(Depth);
	--Depth;

	if (bPretty && Buffer.Last() != '{')
	{
		AppendSpace();
	}

	Buffer.Add('}');
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
	, bQuoted(false)
{
}

const FString& FPsDataFastJsonPointer::GetString(const TCHAR* Source)
{
	if (!bHasString)
	{
		bHasString = true;
		if (!IsEmpty(Source, StartPosition, EndPosition))
		{
			Trim(Source, StartPosition, EndPosition, bQuoted);
			String = JsonStringToString(Source, StartPosition, EndPosition - StartPosition + 1);
		}
	}

	return String;
}

bool FPsDataFastJsonPointer::IsQuoted(const TCHAR* Source)
{
	GetString(Source);
	return bQuoted;
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
	, bTruncated(false)
{
	Pointers.Reserve(100);
	Parse();

	DepthStack.Reserve(10);
}

FPsDataFastJsonDeserializer::FPsDataFastJsonDeserializer(FString&& InJsonString)
	: FPsDataDeserializer()
	, OwnedJsonString(MoveTemp(InJsonString))
	, Source(OwnedJsonString.GetCharArray().GetData())
	, Size(OwnedJsonString.Len())
	, PointerIndex(0)
	, bTruncated(false)
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

	// A document that is a single value ("null", 42, "text") has no structural token at all
	if (Pointers.Num() == 0 && Size > 0 && !IsEmpty(Source, 0, Size - 1))
	{
		Pointers.Add({EPsDataFastJsonToken::Value, 0, Size - 1, 1});
	}

	// Every read looks at Pointers[PointerIndex] and only moves past a token it understood, so this
	// sentinel keeps an empty, truncated or malformed document from being read out of bounds
	Pointers.Add({EPsDataFastJsonToken::End, Size, Size, INDEX_NONE});
}

bool FPsDataFastJsonDeserializer::IsComplete()
{
	SkipComma();
	return !bTruncated && Pointers[PointerIndex].Token == EPsDataFastJsonToken::End;
}

void FPsDataFastJsonDeserializer::SkipComma()
{
	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token == EPsDataFastJsonToken::Comma)
	{
		++PointerIndex;
	}
}

EPsDataDeserializedValueType FPsDataFastJsonDeserializer::NextValueType()
{
	if (!Pointers.IsValidIndex(PointerIndex))
	{
		return EPsDataDeserializedValueType::None;
	}

	SkipComma();
	if (!Pointers.IsValidIndex(PointerIndex))
	{
		return EPsDataDeserializedValueType::None;
	}

	auto& Pointer = Pointers[PointerIndex];
	switch (Pointer.Token)
	{
	case EPsDataFastJsonToken::OpenObject: return EPsDataDeserializedValueType::Object;
	case EPsDataFastJsonToken::OpenArray: return EPsDataDeserializedValueType::Array;
	case EPsDataFastJsonToken::Value: break;
	default: return EPsDataDeserializedValueType::None;
	}

	const FString& Value = Pointer.GetString(Source);
	if (Pointer.IsQuoted(Source))
	{
		return EPsDataDeserializedValueType::String;
	}

	if (Value.Equals(TEXT("null"), ESearchCase::IgnoreCase))
	{
		return EPsDataDeserializedValueType::Null;
	}

	if (Value.Equals(TEXT("true"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("false"), ESearchCase::IgnoreCase))
	{
		return EPsDataDeserializedValueType::Boolean;
	}

	// Whole numbers that fit an int64 travel as int64. The integer parser does not detect overflow, so the
	// range is checked here: 18 digits always fit, 19 fit up to 9223372036854775807 (the minimum is left out)
	const auto Digits = ToStringView(Value).RightChop(Value.StartsWith(TEXT("-")) ? 1 : 0);
	if (PsDataTools::Numbers::IsUnsignedInteger(Digits))
	{
		static const FString MaxInt64(TEXT("9223372036854775807"));
		const FString DigitString(Digits.Len(), Digits.GetData());
		if (Digits.Len() < 19 || (Digits.Len() == 19 && DigitString.Compare(MaxInt64, ESearchCase::CaseSensitive) <= 0))
		{
			return EPsDataDeserializedValueType::Int64;
		}
	}

	if (PsDataTools::Numbers::IsNumber(ToStringView(Value)))
	{
		return EPsDataDeserializedValueType::Double;
	}

	// Lenient like the rest of this parser: an unquoted word is read as a string
	return EPsDataDeserializedValueType::String;
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
	if (Pointer.Token == EPsDataFastJsonToken::CloseArray || Pointer.Token == EPsDataFastJsonToken::End)
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
	const auto NumberOpt = PsDataTools::Numbers::ToNumber<int32>(ToStringView(Value));
	if (!NumberOpt)
	{
		return false;
	}

	OutValue = NumberOpt.GetValue();
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
	const auto NumberOpt = PsDataTools::Numbers::ToNumber<int64>(ToStringView(Value));
	if (!NumberOpt)
	{
		return false;
	}

	OutValue = NumberOpt.GetValue();
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
	const auto NumberOpt = PsDataTools::Numbers::ToNumber<uint8>(ToStringView(Value));
	if (!NumberOpt)
	{
		return false;
	}

	OutValue = NumberOpt.GetValue();
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
	const auto NumberOpt = PsDataTools::Numbers::ToNumber<float>(ToStringView(Value));
	if (!NumberOpt)
	{
		return false;
	}

	OutValue = NumberOpt.GetValue();
	Pointer.Reset();

	++PointerIndex;
	return true;
}

bool FPsDataFastJsonDeserializer::ReadValue(double& OutValue)
{
	SkipComma();

	auto& Pointer = Pointers[PointerIndex];
	if (Pointer.Token != EPsDataFastJsonToken::Value)
	{
		return false;
	}

	// FCString::Atod rounds correctly; the plugin's own parser accumulates powers of ten and drifts in the last digits
	const FString& Value = Pointer.GetString(Source);
	if (!PsDataTools::Numbers::IsNumber(ToStringView(Value)))
	{
		return false;
	}

	OutValue = FCString::Atod(*Value);
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
	const int32 Depth = DepthStack.Pop(PSDATA_NO_SHRINK);
	for (int32 i = PointerIndex; i < Pointers.Num(); ++i)
	{
		const auto& Pointer = Pointers[i];
		if (Pointer.Depth == Depth)
		{
			PointerIndex = i;
			return;
		}
	}

	// A truncated document: nothing after this point can be read
	PointerIndex = Pointers.Num() - 1;
	bTruncated = true;
}

void FPsDataFastJsonDeserializer::PopIndex()
{
}

void FPsDataFastJsonDeserializer::PopArray()
{
	const int32 Depth = DepthStack.Pop(PSDATA_NO_SHRINK);
	for (int32 i = PointerIndex; i < Pointers.Num(); ++i)
	{
		const auto& Pointer = Pointers[i];
		if (Pointer.Token == EPsDataFastJsonToken::CloseArray && Pointer.Depth == Depth)
		{
			PointerIndex = i + 1;
			return;
		}
	}

	// A truncated document: nothing after this point can be read
	PointerIndex = Pointers.Num() - 1;
	bTruncated = true;
}

void FPsDataFastJsonDeserializer::PopObject()
{
	const int32 Depth = DepthStack.Pop(PSDATA_NO_SHRINK);
	for (int32 i = PointerIndex; i < Pointers.Num(); ++i)
	{
		const auto& Pointer = Pointers[i];
		if (Pointer.Token == EPsDataFastJsonToken::CloseObject && Pointer.Depth == Depth)
		{
			PointerIndex = i + 1;
			return;
		}
	}

	// A truncated document: nothing after this point can be read
	PointerIndex = Pointers.Num() - 1;
	bTruncated = true;
}
