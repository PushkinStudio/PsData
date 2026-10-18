// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "Serialize/Stream/PsDataBufferOutputStream.h"
#include "Serialize/Stream/PsDataOutputStream.h"

#include "CoreMinimal.h"
#include "Misc/SecureHash.h"

/***********************************
 * FPsDataMD5Hash
 ***********************************/

struct PSDATA_API FPsDataMD5Hash
{
	FPsDataMD5Hash(FMD5 Md5Gen);

	FString ToString() const;
	int32 ToInt32() const;
	int64 ToInt64() const;
	void GetDigest(uint64& OutA, uint64& OutB) const;

private:
	uint64 A;
	uint64 B;
};

/***********************************
 * FPsDataMD5OutputStream
 ***********************************/

struct PSDATA_API FPsDataMD5OutputStream : public FPsDataOutputStream
{
public:
	FPsDataMD5OutputStream();
	virtual ~FPsDataMD5OutputStream(){};

private:
	FMD5 Md5Gen;
	FPsDataBufferOutputStream OutputSteram;

public:
	FPsDataMD5Hash GetHash();

	virtual void WriteUint8(uint8 Value) override;
	virtual void WriteInt8(int8 Value) override;
	virtual void WriteUint16(uint16 Value) override;
	virtual void WriteInt16(int16 Value) override;
	virtual void WriteUint32(uint32 Value) override;
	virtual void WriteInt32(int32 Value) override;
	virtual void WriteUint64(uint64 Value) override;
	virtual void WriteInt64(int64 Value) override;
	virtual void WriteFloat(float Value) override;
	virtual void WriteBool(bool Value) override;
	virtual void WriteTCHAR(TCHAR Value) override;
	virtual void WriteString(const FString& Value) override;
	virtual void WriteBuffer(const TArray<uint8>& Value) override;
	virtual void WriteBuffer(const uint8* Buffer, int32 Count) override;
	virtual void WriteBuffer(TArray<uint8>&& Value) override;
	virtual int32 Size() const override;
};
