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
	Buffer.Append(reinterpret_cast<const char*>(&Data), sizeof(T));
}

template <>
void Write<FString>(TArray<char>& Buffer, const FString& Data)
{
	const int32 StrLen = sizeof(TCHAR) * Data.Len();
	Buffer.Reserve(Buffer.Num() + FMath::Max(Buffer.GetSlack(), static_cast<int32>(StrLen + sizeof(uint32))));
	Write(Buffer, static_cast<uint32>(Data.Len()));
	Buffer.Append(reinterpret_cast<const char*>(Data.GetCharArray().GetData()), StrLen);
}

template <typename T>
T Read(const TArray<char>& Buffer, uint32& Index)
{
	const T* Data = reinterpret_cast<const T*>(&Buffer.GetData()[Index]);
	Index += sizeof(T);
	return *Data;
}

template <>
FString Read<FString>(const TArray<char>& Buffer, uint32& Index)
{
	uint32 Size = Read<uint32>(Buffer, Index);
	FString Data;
	Data.Append(reinterpret_cast<const TCHAR*>(&Buffer.GetData()[Index]), Size);
	Index += Size * sizeof(TCHAR);
	return Data;
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
	Write(Buffer, EBinaryTokens::Key);
	Write(Buffer, Key);
}

void FPsDataBinarySerializer::WriteArray()
{
	Write(Buffer, EBinaryTokens::ArrayBegin);
}

void FPsDataBinarySerializer::WriteObject()
{
	Write(Buffer, EBinaryTokens::ObjectBegin);
}

void FPsDataBinarySerializer::WriteValue(int32 Value)
{
	Write(Buffer, EBinaryTokens::Value_int32);
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(uint8 Value)
{
	Write(Buffer, EBinaryTokens::Value_uint8);
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(float Value)
{
	Write(Buffer, EBinaryTokens::Value_float);
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(bool Value)
{
	Write(Buffer, EBinaryTokens::Value_bool);
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(const FString& Value)
{
	Write(Buffer, EBinaryTokens::Value_FString);
	Write(Buffer, Value);
}

void FPsDataBinarySerializer::WriteValue(const FName& Value)
{
	Write(Buffer, EBinaryTokens::Value_FName);
	Write(Buffer, Value.ToString());
}

void FPsDataBinarySerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		Write(Buffer, EBinaryTokens::Value_null);
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
	Write(Buffer, EBinaryTokens::ArrayEnd);
}

void FPsDataBinarySerializer::PopObject()
{
	Write(Buffer, EBinaryTokens::ObjectEnd);
}

/***********************************
 * FPsDataBinaryDeserializer
 ***********************************/

FPsDataBinaryDeserializer::FPsDataBinaryDeserializer(const TArray<char>& InBuffer)
	: FPsDataDeserializer()
	, Buffer(InBuffer)
	, BufferIndex(0)
{
}

bool FPsDataBinaryDeserializer::ReadToken(EBinaryTokens Token)
{
	if (Read<EBinaryTokens>(Buffer, BufferIndex) == Token)
	{
		return true;
	}
	else
	{
		BufferIndex -= sizeof(EBinaryTokens);
		return false;
	}
}

bool FPsDataBinaryDeserializer::ReadKey(FString& OutKey)
{
	if (ReadToken(EBinaryTokens::Key))
	{
		OutKey = Read<FString>(Buffer, BufferIndex);
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadIndex()
{
	if (ReadToken(EBinaryTokens::ArrayEnd))
	{
		BufferIndex -= sizeof(EBinaryTokens);
		return false;
	}
	return true;
}

bool FPsDataBinaryDeserializer::ReadArray()
{
	if (ReadToken(EBinaryTokens::ArrayBegin))
	{
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadObject()
{
	if (ReadToken(EBinaryTokens::ObjectBegin))
	{
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(int32& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_int32))
	{
		OutValue = Read<int32>(Buffer, BufferIndex);
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(uint8& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_uint8))
	{
		OutValue = Read<uint8>(Buffer, BufferIndex);
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(float& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_float))
	{
		OutValue = Read<float>(Buffer, BufferIndex);
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(bool& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_bool))
	{
		OutValue = Read<bool>(Buffer, BufferIndex);
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(FString& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_FString))
	{
		OutValue = Read<FString>(Buffer, BufferIndex);
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(FName& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_FName))
	{
		FString String = Read<FString>(Buffer, BufferIndex);
		OutValue = *String;
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator)
{
	if (ReadToken(EBinaryTokens::Value_null))
	{
		OutValue = nullptr;
		return true;
	}
	else if (ReadObject())
	{
		if (OutValue == nullptr)
		{
			OutValue = Allocator();
		}
		OutValue->DataDeserialize(this);
		PopObject();
		return true;
	}
	return false;
}

void FPsDataBinaryDeserializer::PopKey(const FString& Key)
{
}

void FPsDataBinaryDeserializer::PopIndex()
{
}

void FPsDataBinaryDeserializer::PopArray()
{
	check(ReadToken(EBinaryTokens::ArrayEnd));
}

void FPsDataBinaryDeserializer::PopObject()
{
	check(ReadToken(EBinaryTokens::ObjectEnd));
}
