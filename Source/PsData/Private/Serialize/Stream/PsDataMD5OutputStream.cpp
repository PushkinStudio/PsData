// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Serialize/Stream/PsDataMD5OutputStream.h"

void Write(FMD5& Md5Gen, const TArray<uint8>& Buffer)
{
	Md5Gen.Update(Buffer.GetData(), Buffer.Num());
}

void Write(FMD5& Md5Gen, const uint8* Buffer, int32 Count)
{
	Md5Gen.Update(Buffer, Count);
}

/***********************************
 * FPsDataMD5Hash
 ***********************************/

FPsDataMD5Hash::FPsDataMD5Hash(FMD5 Md5Gen)
{
	uint8 Data[16];
	Md5Gen.Final(Data);

	A = (static_cast<uint64>(Data[0]) << 56) | (static_cast<uint64>(Data[1]) << 48) | (static_cast<uint64>(Data[2]) << 40) | (static_cast<uint64>(Data[3]) << 32) |
		(static_cast<uint64>(Data[4]) << 24) | (static_cast<uint64>(Data[5]) << 16) | (static_cast<uint64>(Data[6]) << 8) | (static_cast<uint64>(Data[7]));
	B = (static_cast<uint64>(Data[8]) << 56) | (static_cast<uint64>(Data[9]) << 48) | (static_cast<uint64>(Data[10]) << 40) | (static_cast<uint64>(Data[11]) << 32) |
		(static_cast<uint64>(Data[12]) << 24) | (static_cast<uint64>(Data[13]) << 16) | (static_cast<uint64>(Data[14]) << 8) | (static_cast<uint64>(Data[15]));
}

FString FPsDataMD5Hash::ToString() const
{
	return FString::Printf(TEXT("%016llx%016llx"), A, B);
}

uint32 FPsDataMD5Hash::ToUint32() const
{
	return static_cast<uint32>(A);
}

uint64 FPsDataMD5Hash::ToUint64() const
{
	return A;
}

void FPsDataMD5Hash::GetDigest(uint64& OutA, uint64& OutB) const
{
	OutA = A;
	OutB = B;
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

void FPsDataMD5OutputStream::WriteBuffer(const TArray<uint8>& Value)
{
	Write(Md5Gen, Value);
}

void FPsDataMD5OutputStream::WriteBuffer(const uint8* Buffer, int32 Count)
{
	Write(Md5Gen, Buffer, Count);
}

void FPsDataMD5OutputStream::WriteBuffer(TArray<uint8>&& Value)
{
	Write(Md5Gen, Value);
}

int32 FPsDataMD5OutputStream::Size() const
{
	return 0;
}
