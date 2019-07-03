// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataBufferOutputStream.h"

template <typename T>
void Write(TArray<char>& Buffer, const T& Data)
{
	Buffer.Append(static_cast<const char*>(static_cast<const void*>(&Data)), sizeof(T));
}

template <>
void Write<FString>(TArray<char>& Buffer, const FString& Data)
{
	const int32 Size = Data.Len();
	Write<int32>(Buffer, Size);
	if (Size > 0)
	{
		Buffer.Append(static_cast<const char*>(static_cast<const void*>(Data.GetCharArray().GetData())), Size * sizeof(TCHAR));
	}
}

/***********************************
 * FPsDataBufferOutputStream
 ***********************************/

FPsDataBufferOutputStream::FPsDataBufferOutputStream()
{
}

const TArray<char>& FPsDataBufferOutputStream::GetBuffer()
{
	return Buffer;
}

void FPsDataBufferOutputStream::WriteInt32(int32 Value)
{
	Write<int32>(Buffer, Value);
}

void FPsDataBufferOutputStream::WriteUint8(uint8 Value)
{
	Write<uint8>(Buffer, Value);
}

void FPsDataBufferOutputStream::WriteFloat(float Value)
{
	Write<float>(Buffer, Value);
}

void FPsDataBufferOutputStream::WriteBool(bool Value)
{
	Write<bool>(Buffer, Value);
}

void FPsDataBufferOutputStream::WriteString(const FString& Value)
{
	Write<FString>(Buffer, Value);
}
