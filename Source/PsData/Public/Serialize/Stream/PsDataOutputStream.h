// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/***********************************
 * FPsDataOutputStream
 ***********************************/

struct PSDATA_API FPsDataOutputStream
{
	virtual void WriteUint32(uint32 Value) = 0;
	virtual void WriteInt32(int32 Value) = 0;
	virtual void WriteUint64(uint64 Value) = 0;
	virtual void WriteInt64(int64 Value) = 0;
	virtual void WriteUint8(uint8 Value) = 0;
	virtual void WriteFloat(float Value) = 0;
	virtual void WriteBool(bool Value) = 0;
	virtual void WriteTCHAR(TCHAR Value) = 0;
	virtual void WriteString(const FString& Value) = 0;
	virtual void WriteBuffer(const TArray<uint8>& Value) = 0;
	virtual void WriteBuffer(const uint8* Buffer, int32 Count) = 0;
	virtual void WriteBuffer(TArray<uint8>&& Value) = 0;
	virtual int32 Size() const = 0;
};
