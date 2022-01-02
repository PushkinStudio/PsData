// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Serialize/PsDataFastJsonSerialization.h"

#include "PsData.h"

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

constexpr char CharToEscape[] = {'\\', '\n', '\r', '\t', '"'};
constexpr char EscapedChars[] = {'\\', 'n', 'r', 't', '"'};
constexpr uint32 MaxSupportedEscapeChars = UE_ARRAY_COUNT(CharToEscape);

int32 FindСharToEscape(TCHAR Char)
{
	for (int32 i = 0; i < MaxSupportedEscapeChars; ++i)
	{
		if (Char == CharToEscape[i])
		{
			return i;
		}
	}

	return INDEX_NONE;
}

bool HasСharToEscape(const TCHAR* String, int32 StartPosition, int32 EndPosition)
{
	while (StartPosition < EndPosition)
	{
		if (FindСharToEscape(String[StartPosition]) != INDEX_NONE)
		{
			return true;
		}
		++StartPosition;
	}

	return false;
}

void AppendStringAsJsonString(TArray<TCHAR>& JsonStringCharArray, const TCHAR* String, int32 StartPosition, int32 Count)
{
	JsonStringCharArray.Reserve(JsonStringCharArray.Num() + 2 + 2 * Count);
	JsonStringCharArray.Add('"');

	for (int32 i = StartPosition; i < StartPosition + Count; ++i)
	{
		const auto c = String[i];
		const auto EscapeIndex = FindСharToEscape(c);
		if (EscapeIndex != INDEX_NONE)
		{
			JsonStringCharArray.Add('\\');
			JsonStringCharArray.Add(EscapedChars[EscapeIndex]);
		}
		else
		{
			JsonStringCharArray.Add(c);
		}
	}

	JsonStringCharArray.Add('"');
}

template <uint32 Degree>
constexpr uint64 PowerOfTen()
{
	static_assert(Degree <= std::numeric_limits<uint64>::digits10, "Out of bounds");
	return 10 * PowerOfTen<Degree - 1>();
}

template <>
constexpr uint64 PowerOfTen<0>()
{
	return 1;
}

template <typename T>
typename TEnableIf<std::is_integral<T>::value, void>::Type
AppendNumber(TArray<TCHAR>& JsonStringCharArray, T Value)
{
	constexpr TCHAR ZeroChar = '0';
	constexpr bool bCanBeNegative = TNumericLimits<T>::Min() < 0;
	constexpr int32 Length = (std::numeric_limits<T>::digits10 + 1) + (bCanBeNegative ? 1 : 0);

	int32 Pos = Length;
	TCHAR CharBuffer[Length];

	const bool bNegative = bCanBeNegative && Value < 0;
	if (bNegative)
	{
		Value *= -1;
	}

	do
	{
		CharBuffer[--Pos] = ZeroChar + (Value % 10);
		Value /= 10;
	}
	while (Value > 0);

	if (bNegative)
	{
		CharBuffer[--Pos] = '-';
	}

	JsonStringCharArray.Append(&CharBuffer[Pos], Length - Pos);
}

template <typename T, int32 Precision = 6>
typename TEnableIf<std::is_floating_point<T>::value, void>::Type
AppendNumber(TArray<TCHAR>& JsonStringCharArray, T Value)
{
	constexpr TCHAR ZeroChar = '0';

	auto FloatType = std::fpclassify(Value);
	if (FloatType == FP_NAN || FloatType == FP_INFINITE)
	{
		// ECMA-404
		JsonStringCharArray.Append(TEXT("null"), 4);
	}
	else if (FloatType == FP_ZERO || FloatType == FP_SUBNORMAL)
	{
		JsonStringCharArray.Add(ZeroChar);
	}
	else
	{
		using UintType = typename std::conditional<std::is_base_of<T, float>::value, uint32, uint64>::type;
		constexpr T MaxUintValue = PowerOfTen<std::numeric_limits<T>::digits10>();
		constexpr uint32 InvEpsilon = PowerOfTen<Precision>();
		constexpr T Epsilon = static_cast<T>(1) / static_cast<T>(InvEpsilon);

		/**
		 * Default buffer size: "±" + A(minimum 10 digits) + "." + B(Precision) ≈ 18
		 * Scientific notation buffer size: "±" + A(always 1 digit) + "." + B(Precision) + "e±" + E(maximum 4 digits) ≈ 15
		 */
		constexpr int32 Length = 1 + (std::numeric_limits<UintType>::digits10 + 1) + 1 + Precision;
		int32 Pos = Length;
		TCHAR CharBuffer[Length];

		const bool bNegative = Value < 0;
		if (bNegative)
		{
			Value *= -1;
		}

		const bool bScientificNotation = (Value < Epsilon || Value > MaxUintValue);
		if (bScientificNotation)
		{
			auto Exp = static_cast<int32>(std::floor(std::log10(Value)));
			Value = Value * std::pow(10, -Exp);

			const bool bExpNegative = Exp < 0;
			if (bExpNegative)
			{
				Exp *= -1;
			}

			do
			{
				CharBuffer[--Pos] = ZeroChar + (Exp % 10);
				Exp /= 10;
			}
			while (Exp > 0);

			if (bExpNegative)
			{
				CharBuffer[--Pos] = '-';
			}

			CharBuffer[--Pos] = 'e';
		}

		UintType A = static_cast<UintType>(Value);
		uint32 B = static_cast<uint32>((Value - A) * InvEpsilon + 0.5);

		if (B > 0)
		{
			int32 NumDigits = Precision;
			while (B % 10 == 0)
			{
				B /= 10;
				--NumDigits;
			}

			while (NumDigits > 0)
			{
				CharBuffer[--Pos] = ZeroChar + (B % 10);
				B /= 10;
				--NumDigits;
			}

			CharBuffer[--Pos] = '.';
		}

		do
		{
			CharBuffer[--Pos] = ZeroChar + (A % 10);
			A /= 10;
		}
		while (A > 0);

		if (bNegative)
		{
			CharBuffer[--Pos] = '-';
		}

		JsonStringCharArray.Append(&CharBuffer[Pos], Length - Pos);
	}
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

bool IsNumeric(const FString& Value)
{
	bool bDigit = false;
	bool bDot = false;
	bool bSign = false;
	bool bExp = false;

	for (const auto C : Value)
	{
		if (C >= '0' && C <= '9')
		{
			bDigit = true;
		}
		else if (C == '.')
		{
			if (bDot || bExp)
			{
				return false;
			}

			bDigit = true;
			bDot = true;
		}
		else if (C == '+' || C == '-')
		{
			if (bSign || bDigit)
			{
				return false;
			}

			bSign = true;
		}
		else if (C == 'E' || C == 'e')
		{
			if (bExp || !bDigit)
			{
				return false;
			}

			bDigit = false;
			bSign = false;
			bExp = true;
		}
		else
		{
			return false;
		}
	}

	return bDigit;
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

	Buffer.Add('"');
	Buffer.Append(Key.GetCharArray().GetData(), Key.Len());
	Buffer.Add('"');
	Buffer.Add(':');
}

void FPsDataFastJsonSerializer::WriteArray()
{
	AppendComma();
	AppendValueSpace();

	++Depth;
	Buffer.Add('[');
}

void FPsDataFastJsonSerializer::WriteObject()
{
	AppendComma();
	AppendValueSpace();

	++Depth;
	Buffer.Add('{');
}

void FPsDataFastJsonSerializer::WriteValue(int32 Value)
{
	AppendComma();
	AppendValueSpace();

	AppendNumber(Buffer, Value);
}

void FPsDataFastJsonSerializer::WriteValue(int64 Value)
{
	AppendComma();
	AppendValueSpace();

	AppendNumber(Buffer, Value);
}

void FPsDataFastJsonSerializer::WriteValue(uint8 Value)
{
	AppendComma();
	AppendValueSpace();

	AppendNumber(Buffer, Value);
}

void FPsDataFastJsonSerializer::WriteValue(float Value)
{
	AppendComma();
	AppendValueSpace();

	AppendNumber(Buffer, Value);
}

void FPsDataFastJsonSerializer::WriteValue(bool Value)
{
	AppendComma();
	AppendValueSpace();

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
	AppendValueSpace();

	AppendStringAsJsonString(Buffer, Value.GetCharArray().GetData(), 0, Value.Len());
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
	if (!IsNumeric(Value))
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
	if (!IsNumeric(Value))
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
	if (!IsNumeric(Value))
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
	if (!IsNumeric(Value))
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
	else if (IsNumeric(Value))
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
