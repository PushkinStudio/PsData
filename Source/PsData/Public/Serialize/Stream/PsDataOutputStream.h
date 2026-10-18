// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/***********************************
 * FPsDataOutputStream
 ***********************************/

struct PSDATA_API FPsDataOutputStream
{
	virtual void WriteUint8(uint8 Value) = 0;
	virtual void WriteInt8(int8 Value) = 0;
	virtual void WriteUint16(uint16 Value) = 0;
	virtual void WriteInt16(int16 Value) = 0;
	virtual void WriteUint32(uint32 Value) = 0;
	virtual void WriteInt32(int32 Value) = 0;
	virtual void WriteUint64(uint64 Value) = 0;
	virtual void WriteInt64(int64 Value) = 0;
	virtual void WriteFloat(float Value) = 0;

	/** The double's bits as a uint64, so every stream supports it without its own implementation */
	virtual void WriteDouble(double Value)
	{
		uint64 Bits;
		FMemory::Memcpy(&Bits, &Value, sizeof(Bits));
		WriteUint64(Bits);
	}

	virtual void WriteBool(bool Value) = 0;
	virtual void WriteTCHAR(TCHAR Value) = 0;
	virtual void WriteString(const FString& Value) = 0;
	virtual void WriteBuffer(const TArray<uint8>& Value) = 0;
	virtual void WriteBuffer(const uint8* Buffer, int32 Count) = 0;
	virtual void WriteBuffer(TArray<uint8>&& Value) = 0;
	virtual int32 Size() const = 0;
};
