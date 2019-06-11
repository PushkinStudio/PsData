// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataMD5OutputStream.h"

template <typename T>
void Write(FMD5& Md5Gen, const T& Data)
{
	Md5Gen.Update(static_cast<const uint8*>(static_cast<const void*>(&Data)), sizeof(T));
}

template <>
void Write<FString>(FMD5& Md5Gen, const FString& Data)
{
	Write<int32>(Md5Gen, Data.Len());
	Md5Gen.Update(static_cast<const uint8*>(static_cast<const void*>(Data.GetCharArray().GetData())), Data.Len() * sizeof(TCHAR));
}

/***********************************
 * FPsDataMD5OutputStream
 ***********************************/

FPsDataMD5OutputStream::FPsDataMD5OutputStream()
{
}

FString FPsDataMD5OutputStream::GetHash()
{
	uint8 Digest[16];
	Md5Gen.Final(Digest);
	return FString::Printf(
		TEXT("%08x%08x%08x%08x"),
		(static_cast<uint32>(Digest[0]) << 24) | (static_cast<uint32>(Digest[1]) << 16) | (static_cast<uint32>(Digest[2]) << 8) | static_cast<uint32>(Digest[3]),
		(static_cast<uint32>(Digest[4]) << 24) | (static_cast<uint32>(Digest[5]) << 16) | (static_cast<uint32>(Digest[6]) << 8) | static_cast<uint32>(Digest[7]),
		(static_cast<uint32>(Digest[8]) << 24) | (static_cast<uint32>(Digest[9]) << 16) | (static_cast<uint32>(Digest[10]) << 8) | static_cast<uint32>(Digest[11]),
		(static_cast<uint32>(Digest[12]) << 24) | (static_cast<uint32>(Digest[13]) << 16) | (static_cast<uint32>(Digest[14]) << 8) | static_cast<uint32>(Digest[15]));
}

void FPsDataMD5OutputStream::WriteInt32(int32 Value)
{
	Write<int32>(Md5Gen, Value);
}

void FPsDataMD5OutputStream::WriteUint8(uint8 Value)
{
	Write<uint8>(Md5Gen, Value);
}

void FPsDataMD5OutputStream::WriteFloat(float Value)
{
	Write<float>(Md5Gen, Value);
}

void FPsDataMD5OutputStream::WriteBool(bool Value)
{
	Write<bool>(Md5Gen, Value);
}

void FPsDataMD5OutputStream::WriteString(const FString& Value)
{
	Write<FString>(Md5Gen, Value);
}
