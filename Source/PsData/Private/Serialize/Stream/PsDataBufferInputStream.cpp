// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataBufferInputStream.h"

/***********************************
 * FPsDataBufferInputStream
 ***********************************/

FPsDataBufferInputStream::FPsDataBufferInputStream(const TArray<uint8>& InBuffer)
	: Buffer(InBuffer)
	, Index(0)
	, PrevIndex(-1)
{
}

uint32 FPsDataBufferInputStream::ReadUint32()
{
	CheckRange();
	PrevIndex = Index;
	const uint32 b0 = (static_cast<uint32>(Buffer[Index + 0]) << 24);
	const uint32 b1 = (static_cast<uint32>(Buffer[Index + 1]) << 16);
	const uint32 b2 = (static_cast<uint32>(Buffer[Index + 2]) << 8);
	const uint32 b3 = (static_cast<uint32>(Buffer[Index + 3]) << 0);
	Index += 4;
	return b0 | b1 | b2 | b3;
}

int32 FPsDataBufferInputStream::ReadInt32()
{
	const uint32 Value = ReadUint32();
	if ((Value & 0x80000000) == 0)
	{
		return static_cast<int32>(Value);
	}
	else
	{
		return static_cast<int32>(Value ^ 0x80000000) * -1;
	}
}

uint64 FPsDataBufferInputStream::ReadUint64()
{
	CheckRange();
	PrevIndex = Index;
	const uint64 b0 = (static_cast<uint64>(Buffer[Index + 0]) << 56);
	const uint64 b1 = (static_cast<uint64>(Buffer[Index + 1]) << 48);
	const uint64 b2 = (static_cast<uint64>(Buffer[Index + 2]) << 40);
	const uint64 b3 = (static_cast<uint64>(Buffer[Index + 3]) << 32);
	const uint64 b4 = (static_cast<uint64>(Buffer[Index + 4]) << 24);
	const uint64 b5 = (static_cast<uint64>(Buffer[Index + 5]) << 16);
	const uint64 b6 = (static_cast<uint64>(Buffer[Index + 6]) << 8);
	const uint64 b7 = (static_cast<uint64>(Buffer[Index + 7]) << 0);
	Index += 8;
	return b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7;
}

int64 FPsDataBufferInputStream::ReadInt64()
{
	const uint64 Value = ReadUint64();
	if ((Value & 0x8000000000000000) == 0)
	{
		return static_cast<int64>(Value);
	}
	else
	{
		return static_cast<int64>(Value ^ 0x8000000000000000) * -1;
	}
}

uint8 FPsDataBufferInputStream::ReadUint8()
{
	CheckRange();
	PrevIndex = Index;
	const auto b0 = Buffer[Index];
	Index += 1;
	return b0;
}

float FPsDataBufferInputStream::ReadFloat()
{
	const uint32 Value = ReadUint32();
	return *reinterpret_cast<const float*>(&Value);
}

bool FPsDataBufferInputStream::ReadBool()
{
	const auto b0 = ReadUint8();
	return b0 == 0x01;
}

TCHAR FPsDataBufferInputStream::ReadTCHAR()
{
	const auto Codepoint = ReadUint32();
	return static_cast<TCHAR>(Codepoint);
}

FString FPsDataBufferInputStream::ReadString()
{
	const int32 RealPrevIndex = Index;
	const uint32 Len = ReadUint32();
	FString Result;
	if (Len > 0)
	{
		Result.Reserve(Len);
		for (uint32 i = 0; i < Len; ++i)
		{
			Result.AppendChar(ReadTCHAR());
		}
	}
	PrevIndex = RealPrevIndex;
	return Result;
}

bool FPsDataBufferInputStream::HasData()
{
	return Index < Buffer.Num();
}

void FPsDataBufferInputStream::ShiftBack()
{
	check(PrevIndex >= 0);
	Index = PrevIndex;
	PrevIndex = -1;
}

void FPsDataBufferInputStream::CheckRange()
{
	check(Index < Buffer.Num());
}
