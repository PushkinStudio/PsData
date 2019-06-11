// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataBufferInputStream.h"

template <typename T>
T Read(const TArray<char>& Buffer, int32& Index)
{
	const T* Data = static_cast<const T*>(static_cast<const void*>(&Buffer.GetData()[Index]));
	Index += sizeof(T);
	return *Data;
}

template <>
FString Read<FString>(const TArray<char>& Buffer, int32& Index)
{
	int32 Size = Read<int32>(Buffer, Index);
	FString Data;
	Data.Append(static_cast<const TCHAR*>(static_cast<const void*>(&Buffer.GetData()[Index])), Size);
	Index += Size * sizeof(TCHAR);
	return Data;
}

/***********************************
 * FPsDataBufferInputStream
 ***********************************/

FPsDataBufferInputStream::FPsDataBufferInputStream(const TArray<char>& InBuffer)
	: Buffer(InBuffer)
	, Index(0)
{
}

int32 FPsDataBufferInputStream::ReadInt32()
{
	return Read<int32>(Buffer, Index);
}

uint8 FPsDataBufferInputStream::ReadUint8()
{
	return Read<uint8>(Buffer, Index);
}

float FPsDataBufferInputStream::ReadFloat()
{
	return Read<float>(Buffer, Index);
}

bool FPsDataBufferInputStream::ReadBool()
{
	return Read<bool>(Buffer, Index);
}

FString FPsDataBufferInputStream::ReadString()
{
	return Read<FString>(Buffer, Index);
}

int32 FPsDataBufferInputStream::GetIndex()
{
	return Index;
}

void FPsDataBufferInputStream::SetIndex(int32 InIndex)
{
	Index = InIndex;
}

void FPsDataBufferInputStream::AddOffset(int32 Offset)
{
	Index += Offset;
}

void FPsDataBufferInputStream::SubtractOffset(int32 Offset)
{
	Index -= Offset;
}
