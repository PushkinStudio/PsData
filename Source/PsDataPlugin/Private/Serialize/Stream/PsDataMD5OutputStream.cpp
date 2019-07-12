// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/Stream/PsDataMD5OutputStream.h"

void Write(FMD5& Md5Gen, const TArray<uint8>& Buffer)
{
	Md5Gen.Update(Buffer.GetData(), Buffer.Num());
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
	const uint32 i0 = (static_cast<uint32>(Digest[0]) << 24) | (static_cast<uint32>(Digest[1]) << 16) | (static_cast<uint32>(Digest[2]) << 8) | static_cast<uint32>(Digest[3]);
	const uint32 i1 = (static_cast<uint32>(Digest[4]) << 24) | (static_cast<uint32>(Digest[5]) << 16) | (static_cast<uint32>(Digest[6]) << 8) | static_cast<uint32>(Digest[7]);
	const uint32 i2 = (static_cast<uint32>(Digest[8]) << 24) | (static_cast<uint32>(Digest[9]) << 16) | (static_cast<uint32>(Digest[10]) << 8) | static_cast<uint32>(Digest[11]);
	const uint32 i3 = (static_cast<uint32>(Digest[12]) << 24) | (static_cast<uint32>(Digest[13]) << 16) | (static_cast<uint32>(Digest[14]) << 8) | static_cast<uint32>(Digest[15]);
	return FString::Printf(TEXT("%08x%08x%08x%08x"), i0, i1, i2, i3);
}

uint32 FPsDataMD5OutputStream::GetHashAsUint32()
{
	uint8 Digest[16];
	Md5Gen.Final(Digest);
	const uint32 i0 = (static_cast<uint32>(Digest[0]) << 24) | (static_cast<uint32>(Digest[1]) << 16) | (static_cast<uint32>(Digest[2]) << 8) | static_cast<uint32>(Digest[3]);
	return i0;
}

uint64 FPsDataMD5OutputStream::GetHashAsUint64()
{
	uint8 Digest[16];
	Md5Gen.Final(Digest);
	const uint32 i0 = (static_cast<uint32>(Digest[0]) << 24) | (static_cast<uint32>(Digest[1]) << 16) | (static_cast<uint32>(Digest[2]) << 8) | static_cast<uint32>(Digest[3]);
	const uint32 i1 = (static_cast<uint32>(Digest[4]) << 24) | (static_cast<uint32>(Digest[5]) << 16) | (static_cast<uint32>(Digest[6]) << 8) | static_cast<uint32>(Digest[7]);
	return (static_cast<uint64>(i0) << 32) | static_cast<uint32>(i1);
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
