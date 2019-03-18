// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataBinarySerialization.h"

#include "PsData.h"
#include "PsDataUtils.h"

/***********************************
 * Buffer
 ***********************************/

void Reserve(TArray<char>& Buffer)
{
	const int32 Allocation = 10 * 1024;
	if (Buffer.GetSlack() < 1024)
	{
		Buffer.Reserve(Buffer.Num() + Allocation);
	}
}

template <typename T>
void Write(TArray<char>& Buffer, const T& Data)
{
	Reserve(Buffer);
	const uint32 Size = sizeof(T);
	char* Bytes = (char*)&Data;
	for (int32 i = 0; i < Size; ++i)
	{
		Buffer.Add(Bytes[i]);
	}
}

template <>
void Write<FString>(TArray<char>& Buffer, const FString& Data)
{
	const int32 MaxSize = (Data.Len() + 1) * 4;
	if (Buffer.GetSlack() <= MaxSize)
	{
		Buffer.Reserve(Buffer.Num() + MaxSize);
	}
	ANSICHAR Subbuffer[4];
	for (int32 i = 0; i < Data.Len(); ++i)
	{
		int32 Count = FTCHARToUTF8_Convert::Utf8FromCodepoint(Data[i], Subbuffer, sizeof(Subbuffer));
		for (int32 j = 0; j < Count; ++j)
		{
			Buffer.Add(Subbuffer[j]);
		}
	}
	Write(Buffer, int32(0)); // end
}

/***********************************
 * FBinaryDataSerializer
 ***********************************/

FPsDataBinarySerializer::FPsDataBinarySerializer()
	: FPsDataSerializer()
{
	Reserve(Buffer);
}

const TArray<char>& FPsDataBinarySerializer::GetBuffer() const
{
	return Buffer;
}

void FPsDataBinarySerializer::WriteKey(const FString& Key)
{
	Write(Buffer, Key);
}

void FPsDataBinarySerializer::WriteArray()
{
	Write(Buffer, static_cast<uint8>(1));
}

void FPsDataBinarySerializer::WriteObject()
{
	Write(Buffer, static_cast<uint8>(2));
}

void FPsDataBinarySerializer::WriteValue(int32 Value)
{
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(uint8 Value)
{
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(float Value)
{
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(bool Value)
{
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(const FString& Value)
{
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(const FName& Value)
{
	Write(Buffer, Value.ToString());
}

void FPsDataBinarySerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		Write(Buffer, static_cast<uint8>(0));
	}
	else
	{
		WriteObject();
		Value->DataSerialize(this);
		PopObject();
	}
}

void FPsDataBinarySerializer::PopKey(const FString& Key)
{
}

void FPsDataBinarySerializer::PopArray()
{
}

void FPsDataBinarySerializer::PopObject()
{
}
