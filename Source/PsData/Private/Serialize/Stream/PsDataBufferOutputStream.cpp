// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Serialize/Stream/PsDataBufferOutputStream.h"

/***********************************
 * FPsDataBufferOutputStream
 ***********************************/

FPsDataBufferOutputStream::FPsDataBufferOutputStream()
{
}

TArray<uint8>& FPsDataBufferOutputStream::GetBuffer()
{
	return Buffer;
}

void FPsDataBufferOutputStream::Reset()
{
	Buffer.Reset();
}

void FPsDataBufferOutputStream::Reserve(int32 NumBytes)
{
	Buffer.Reserve(NumBytes);
}

void FPsDataBufferOutputStream::WriteUint32(uint32 Value)
{
	Buffer.Push(static_cast<uint8>(Value >> 24));
	Buffer.Push(static_cast<uint8>(Value >> 16));
	Buffer.Push(static_cast<uint8>(Value >> 8));
	Buffer.Push(static_cast<uint8>(Value));
}

void FPsDataBufferOutputStream::WriteInt32(int32 Value)
{
	if (Value < 0)
	{
		WriteUint32(static_cast<uint32>(Value * -1) | 0x80000000);
	}
	else
	{
		WriteUint32(static_cast<uint32>(Value));
	}
}

void FPsDataBufferOutputStream::WriteUint64(uint64 Value)
{
	Buffer.Push(static_cast<uint8>(Value >> 56));
	Buffer.Push(static_cast<uint8>(Value >> 48));
	Buffer.Push(static_cast<uint8>(Value >> 40));
	Buffer.Push(static_cast<uint8>(Value >> 32));
	Buffer.Push(static_cast<uint8>(Value >> 24));
	Buffer.Push(static_cast<uint8>(Value >> 16));
	Buffer.Push(static_cast<uint8>(Value >> 8));
	Buffer.Push(static_cast<uint8>(Value));
}

void FPsDataBufferOutputStream::WriteInt64(int64 Value)
{
	if (Value < 0)
	{
		WriteUint64(static_cast<uint64>(Value * -1) | 0x8000000000000000);
	}
	else
	{
		WriteUint64(static_cast<uint64>(Value));
	}
}

void FPsDataBufferOutputStream::WriteUint8(uint8 Value)
{
	Buffer.Push(Value);
}

void FPsDataBufferOutputStream::WriteFloat(float Value)
{
	WriteUint32(*reinterpret_cast<uint32*>(&Value));
}

void FPsDataBufferOutputStream::WriteBool(bool Value)
{
	WriteUint8(Value ? 0x01 : 0x00);
}

void FPsDataBufferOutputStream::WriteTCHAR(TCHAR Value)
{
	const auto Codepoint = static_cast<uint32>(Value);
	WriteUint32(Codepoint);
}

void FPsDataBufferOutputStream::WriteString(const FString& Value)
{
	const uint32 Len = static_cast<uint32>(Value.Len());
	WriteUint32(Len);
	if (Len > 0)
	{
		for (uint32 i = 0; i < Len; ++i)
		{
			WriteTCHAR(Value[i]);
		}
	}
}

void FPsDataBufferOutputStream::WriteBuffer(const TArray<uint8>& Value)
{
	Buffer.Append(Value);
}

void FPsDataBufferOutputStream::WriteBuffer(const uint8* Value, int32 Count)
{
	Buffer.Append(Value, Count);
}

void FPsDataBufferOutputStream::WriteBuffer(TArray<uint8>&& Value)
{
	Buffer.Append(std::move(Value));
}

int32 FPsDataBufferOutputStream::Size() const
{
	return Buffer.Num();
}