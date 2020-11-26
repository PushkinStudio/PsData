// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

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
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Key));
	OutputStream->WriteString(Key);
}

void FPsDataBinarySerializer::WriteArray()
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::ArrayBegin));
}

void FPsDataBinarySerializer::WriteObject()
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::ObjectBegin));
}

void FPsDataBinarySerializer::WriteValue(int32 Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_int32));
	OutputStream->WriteInt32(Value);
}

void FPsDataBinarySerializer::WriteValue(int64 Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_int64));
	OutputStream->WriteInt64(Value);
}

void FPsDataBinarySerializer::WriteValue(uint8 Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_uint8));
	OutputStream->WriteUint8(Value);
}

void FPsDataBinarySerializer::WriteValue(float Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_float));
	OutputStream->WriteFloat(Value);
}

void FPsDataBinarySerializer::WriteValue(bool Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_bool));
	OutputStream->WriteBool(Value);
}

void FPsDataBinarySerializer::WriteValue(const FString& Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_FString));
	OutputStream->WriteString(Value);
}

void FPsDataBinarySerializer::WriteValue(const FName& Value)
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_FName));
	OutputStream->WriteString(Value.ToString());
}

void FPsDataBinarySerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_null));
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
}

void FPsDataBinarySerializer::PopArray()
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::ArrayEnd));
}

void FPsDataBinarySerializer::PopObject()
{
	OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::ObjectEnd));
}

/***********************************
 * FPsDataBinaryDeserializer
 ***********************************/

FPsDataBinaryDeserializer::FPsDataBinaryDeserializer(TSharedRef<FPsDataInputStream> InInputStream)
	: FPsDataDeserializer()
	, InputStream(InInputStream)
{
}

bool FPsDataBinaryDeserializer::ReadToken(EBinaryTokens Token)
{
	if (!InputStream->HasData())
	{
		return false;
	}

	if (InputStream->ReadUint8() == static_cast<uint8>(Token))
	{
		return true;
	}
	else
	{
		InputStream->ShiftBack();
		return false;
	}
}

bool FPsDataBinaryDeserializer::ReadKey(FString& OutKey)
{
	if (ReadToken(EBinaryTokens::Key))
	{
		OutKey = InputStream->ReadString();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadIndex()
{
	if (ReadToken(EBinaryTokens::ArrayEnd))
	{
		InputStream->ShiftBack();
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
		OutValue = InputStream->ReadInt32();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(int64& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_int64))
	{
		OutValue = InputStream->ReadInt64();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(uint8& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_uint8))
	{
		OutValue = InputStream->ReadUint8();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(float& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_float))
	{
		OutValue = InputStream->ReadFloat();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(bool& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_bool))
	{
		OutValue = InputStream->ReadBool();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(FString& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_FString))
	{
		OutValue = InputStream->ReadString();
		return true;
	}
	return false;
}

bool FPsDataBinaryDeserializer::ReadValue(FName& OutValue)
{
	if (ReadToken(EBinaryTokens::Value_FName))
	{
		FString String = InputStream->ReadString();
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

		PsDataTools::FPsDataFriend::Deserialize(OutValue, this);

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
	const bool bSuccess = ReadToken(EBinaryTokens::ArrayEnd);
	check(bSuccess);
}

void FPsDataBinaryDeserializer::PopObject()
{
	const bool bSuccess = ReadToken(EBinaryTokens::ObjectEnd);
	check(bSuccess);
}
