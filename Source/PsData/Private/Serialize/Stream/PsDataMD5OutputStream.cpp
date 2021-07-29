// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataMD5OutputStream.h"

void Write(FMD5& Md5Gen, const TArray<uint8>& Buffer)
{
	Md5Gen.Update(Buffer.GetData(), Buffer.Num());
}

/***********************************
* FPsDataMD5Hash
***********************************/

FPsDataMD5Hash::FPsDataMD5Hash(FMD5 Md5Gen)
{
	Digest.AddUninitialized(16);
	Md5Gen.Final(Digest.GetData());
}

const TArray<uint8>& FPsDataMD5Hash::GetDigest()
{
	return Digest;
}

FString FPsDataMD5Hash::ToString()
{
	uint8* Data = Digest.GetData();
	const uint32 i0 = (static_cast<uint32>(Data[0]) << 24) | (static_cast<uint32>(Data[1]) << 16) | (static_cast<uint32>(Data[2]) << 8) | static_cast<uint32>(Data[3]);
	const uint32 i1 = (static_cast<uint32>(Data[4]) << 24) | (static_cast<uint32>(Data[5]) << 16) | (static_cast<uint32>(Data[6]) << 8) | static_cast<uint32>(Data[7]);
	const uint32 i2 = (static_cast<uint32>(Data[8]) << 24) | (static_cast<uint32>(Data[9]) << 16) | (static_cast<uint32>(Data[10]) << 8) | static_cast<uint32>(Data[11]);
	const uint32 i3 = (static_cast<uint32>(Data[12]) << 24) | (static_cast<uint32>(Data[13]) << 16) | (static_cast<uint32>(Data[14]) << 8) | static_cast<uint32>(Data[15]);
	return FString::Printf(TEXT("%08x%08x%08x%08x"), i0, i1, i2, i3);
}

uint32 FPsDataMD5Hash::ToUint32()
{
	uint8* Data = Digest.GetData();
	const uint32 i0 = (static_cast<uint32>(Data[0]) << 24) | (static_cast<uint32>(Data[1]) << 16) | (static_cast<uint32>(Data[2]) << 8) | static_cast<uint32>(Data[3]);
	return i0;
}

uint64 FPsDataMD5Hash::ToUint64()
{
	uint8* Data = Digest.GetData();
	const uint32 i0 = (static_cast<uint32>(Data[0]) << 24) | (static_cast<uint32>(Data[1]) << 16) | (static_cast<uint32>(Data[2]) << 8) | static_cast<uint32>(Data[3]);
	const uint32 i1 = (static_cast<uint32>(Data[4]) << 24) | (static_cast<uint32>(Data[5]) << 16) | (static_cast<uint32>(Data[6]) << 8) | static_cast<uint32>(Data[7]);
	return (static_cast<uint64>(i0) << 32) | static_cast<uint64>(i1);
}

/***********************************
 * FPsDataMD5OutputStream
 ***********************************/

FPsDataMD5OutputStream::FPsDataMD5OutputStream()
{
}

FPsDataMD5Hash FPsDataMD5OutputStream::GetHash()
{
	return {Md5Gen};
}

void FPsDataMD5OutputStream::WriteUint32(uint32 Value)
{
	OutputSteram.WriteUint32(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteInt32(int32 Value)
{
	OutputSteram.WriteInt32(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteUint64(uint64 Value)
{
	OutputSteram.WriteUint64(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteInt64(int64 Value)
{
	OutputSteram.WriteInt64(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteUint8(uint8 Value)
{
	OutputSteram.WriteUint8(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteFloat(float Value)
{
	OutputSteram.WriteFloat(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteBool(bool Value)
{
	OutputSteram.WriteBool(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteTCHAR(TCHAR Value)
{
	OutputSteram.WriteTCHAR(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}

void FPsDataMD5OutputStream::WriteString(const FString& Value)
{
	OutputSteram.WriteString(Value);
	Write(Md5Gen, OutputSteram.GetBuffer());
	OutputSteram.Reset();
}
