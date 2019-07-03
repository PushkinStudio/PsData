// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataBufferInputStream.h"

template <typename T>
T Read(const TArray<char>& Buffer, int32& Index)
{
	const int32 BytesCount = sizeof(T);
	check(Buffer.Num() >= (Index + BytesCount));
	auto Data = static_cast<const T*>(static_cast<const void*>(&Buffer.GetData()[Index]));
	Index += BytesCount;
	return *Data;
}

template <>
FString Read<FString>(const TArray<char>& Buffer, int32& Index)
{
	const int32 Size = Read<int32>(Buffer, Index);
	const int32 BytesCount = Size * sizeof(TCHAR);
	if (BytesCount > 0)
	{
		check(Buffer.Num() >= (Index + BytesCount));
		FString Data;
		Data.Append(static_cast<const TCHAR*>(static_cast<const void*>(&Buffer.GetData()[Index])), Size);
		Index += BytesCount;
		return Data;
	}
	else
	{
		return TEXT("");
	}
}

/***********************************
 * FPsDataBufferInputStream
 ***********************************/

FPsDataBufferInputStream::FPsDataBufferInputStream(const TArray<char>& InBuffer)
	: Buffer(InBuffer)
	, Index(0)
	, PrevIndex(-1)
{
}

int32 FPsDataBufferInputStream::ReadInt32()
{
	PrevIndex = Index;
	return Read<int32>(Buffer, Index);
}

uint8 FPsDataBufferInputStream::ReadUint8()
{
	PrevIndex = Index;
	return Read<uint8>(Buffer, Index);
}

float FPsDataBufferInputStream::ReadFloat()
{
	PrevIndex = Index;
	return Read<float>(Buffer, Index);
}

bool FPsDataBufferInputStream::ReadBool()
{
	PrevIndex = Index;
	return Read<bool>(Buffer, Index);
}

FString FPsDataBufferInputStream::ReadString()
{
	PrevIndex = Index;
	return Read<FString>(Buffer, Index);
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
