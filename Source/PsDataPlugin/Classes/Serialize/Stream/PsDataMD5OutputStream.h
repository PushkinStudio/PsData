// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/Stream/PsDataBufferOutputStream.h"
#include "Serialize/Stream/PsDataOutputStream.h"

#include "Core/Public/Misc/SecureHash.h"
#include "CoreMinimal.h"

/***********************************
* FPsDataMD5Hash
***********************************/

struct PSDATAPLUGIN_API FPsDataMD5Hash
{
	FPsDataMD5Hash(FMD5 Md5Gen);

	const TArray<uint8>& GetDigest();
	FString ToString();
	uint32 ToUint32();
	uint64 ToUint64();

private:
	TArray<uint8> Digest;
};

/***********************************
 * FPsDataMD5OutputStream
 ***********************************/

struct PSDATAPLUGIN_API FPsDataMD5OutputStream : public FPsDataOutputStream
{
public:
	FPsDataMD5OutputStream();
	virtual ~FPsDataMD5OutputStream(){};

private:
	FMD5 Md5Gen;
	FPsDataBufferOutputStream OutputSteram;

public:
	FPsDataMD5Hash GetHash();

	virtual void WriteUint32(uint32 Value) override;
	virtual void WriteInt32(int32 Value) override;
	virtual void WriteUint64(uint64 Value) override;
	virtual void WriteInt64(int64 Value) override;
	virtual void WriteUint8(uint8 Value) override;
	virtual void WriteFloat(float Value) override;
	virtual void WriteBool(bool Value) override;
	virtual void WriteTCHAR(TCHAR Value) override;
	virtual void WriteString(const FString& Value) override;
};
