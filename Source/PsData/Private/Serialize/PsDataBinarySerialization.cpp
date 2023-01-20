// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Serialize/PsDataBinarySerialization.h"

#include "PsData.h"

/***********************************
 * FBinaryDataSerializer
 ***********************************/

FPsDataBinarySerializer::FPsDataBinarySerializer(TSharedRef<FPsDataOutputStream> InOutputStream)
	: OutputStream(InOutputStream)
{
}

TSharedRef<FPsDataOutputStream> FPsDataBinarySerializer::GetOutputStream() const
{
	return OutputStream;
}

void FPsDataBinarySerializer::WriteKey(const FString& Key)
{
	OutputStream->WriteUint8(EBinaryTokens::KeyBegin);
	OutputStream->WriteString(Key);
}

void FPsDataBinarySerializer::WriteArray()
{
	OutputStream->WriteUint8(EBinaryTokens::ArrayBegin);
}

void FPsDataBinarySerializer::WriteObject()
{
	OutputStream->WriteUint8(EBinaryTokens::ObjectBegin);
}

void FPsDataBinarySerializer::WriteValue(int32 Value)
{
	OutputStream->WriteUint8(EBinaryTokens::Value_int32);
	OutputStream->WriteInt32(Value);
}

void FPsDataBinarySerializer::WriteValue(int64 Value)
{
	OutputStream->WriteUint8(EBinaryTokens::Value_int64);
	OutputStream->WriteInt64(Value);
}

void FPsDataBinarySerializer::WriteValue(uint8 Value)
{
	OutputStream->WriteUint8(EBinaryTokens::Value_uint8);
	OutputStream->WriteUint8(Value);
}

void FPsDataBinarySerializer::WriteValue(float Value)
{
	OutputStream->WriteUint8(EBinaryTokens::Value_float);
	OutputStream->WriteFloat(Value);
}

void FPsDataBinarySerializer::WriteValue(bool Value)
{
	OutputStream->WriteUint8(EBinaryTokens::Value_bool);
	OutputStream->WriteBool(Value);
}

void FPsDataBinarySerializer::WriteValue(const FString& Value)
{
	OutputStream->WriteUint8(EBinaryTokens::Value_FString);
	OutputStream->WriteString(Value);
}

void FPsDataBinarySerializer::WriteValue(const FName& Value)
{
	FString StringValue = Value.ToString();
	StringValue.ToLowerInline();
	OutputStream->WriteUint8(EBinaryTokens::Value_FName);
	OutputStream->WriteString(StringValue);
}

void FPsDataBinarySerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		OutputStream->WriteUint8(EBinaryTokens::Value_null);
	}
	else
	{
		WriteObject();
		PsDataTools::FPsDataFriend::Serialize(Value, this);
		PopObject();
	}
}

void FPsDataBinarySerializer::PopKey(const FString& Key)
{
	OutputStream->WriteUint8(EBinaryTokens::KeyEnd);
}

void FPsDataBinarySerializer::PopArray()
{
	OutputStream->WriteUint8(EBinaryTokens::ArrayEnd);
}

void FPsDataBinarySerializer::PopObject()
{
	OutputStream->WriteUint8(EBinaryTokens::ObjectEnd);
}

/***********************************
 * FPsDataBinaryDeserializer
 ***********************************/

FPsDataBinaryDeserializer::FPsDataBinaryDeserializer(TSharedRef<FPsDataInputStream> InInputStream)
	: FPsDataDeserializer()
	, InputStream(InInputStream)
{
}

TSharedRef<FPsDataInputStream> FPsDataBinaryDeserializer::GetInputStream() const
{
	return InputStream;
}

uint8 FPsDataBinaryDeserializer::ReadToken()
{
	if (!InputStream->HasData())
	{
		return EBinaryTokens::Null;
	}

	return InputStream->ReadUint8();
}

bool FPsDataBinaryDeserializer::CheckToken(uint8 Token)
{
	if (Token == ReadToken())
	{
		return true;
	}

	InputStream->ShiftBack();
	return false;
}

bool FPsDataBinaryDeserializer::ReadKey(FString& OutKey)
{
	if (CheckToken(EBinaryTokens::KeyBegin))
	{
		OutKey = InputStream->ReadString();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadIndex()
{
	if (CheckToken(EBinaryTokens::ArrayEnd))
	{
		InputStream->ShiftBack();
		return false;
	}
	return true;
}

bool FPsDataBinaryDeserializer::ReadArray()
{
	if (CheckToken(EBinaryTokens::ArrayBegin))
	{
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadObject()
{
	if (CheckToken(EBinaryTokens::ObjectBegin))
	{
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(int32& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_int32))
	{
		OutValue = InputStream->ReadInt32();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(int64& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_int64))
	{
		OutValue = InputStream->ReadInt64();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(uint8& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_uint8))
	{
		OutValue = InputStream->ReadUint8();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(float& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_float))
	{
		OutValue = InputStream->ReadFloat();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(bool& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_bool))
	{
		OutValue = InputStream->ReadBool();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(FString& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_FString))
	{
		OutValue = InputStream->ReadString();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(FName& OutValue)
{
	if (CheckToken(EBinaryTokens::Value_FName))
	{
		const FString String = InputStream->ReadString();
		OutValue = *String;
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator)
{
	if (CheckToken(EBinaryTokens::Value_null))
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

		PsDataTools::FPsDataFriend::Deserialize(OutValue, this);

		PopObject();

		return true;
	}
	return false;
}

void FPsDataBinaryDeserializer::PopKey(const FString& Key)
{
	const bool bSuccess = CheckToken(EBinaryTokens::KeyEnd);
	check(bSuccess);
}

void FPsDataBinaryDeserializer::PopIndex()
{
}

void FPsDataBinaryDeserializer::PopArray()
{
	const bool bSuccess = CheckToken(EBinaryTokens::ArrayEnd);
	check(bSuccess);
}

void FPsDataBinaryDeserializer::PopObject()
{
	const bool bSuccess = CheckToken(EBinaryTokens::ObjectEnd);
	check(bSuccess);
}
