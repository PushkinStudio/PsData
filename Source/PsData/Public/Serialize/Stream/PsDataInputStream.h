// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/***********************************
 * FPsDataInputStream
 ***********************************/

struct PSDATA_API FPsDataInputStream
{
	virtual uint8 ReadUint8() = 0;
	virtual int8 ReadInt8() = 0;
	virtual uint16 ReadUint16() = 0;
	virtual int16 ReadInt16() = 0;
	virtual uint32 ReadUint32() = 0;
	virtual int32 ReadInt32() = 0;
	virtual uint64 ReadUint64() = 0;
	virtual int64 ReadInt64() = 0;
	virtual float ReadFloat() = 0;
	virtual bool ReadBool() = 0;
	virtual TCHAR ReadTCHAR() = 0;
	virtual FString ReadString() = 0;
	virtual bool HasData() = 0;
	virtual void ShiftBack() = 0;
	virtual void SetPosition(int32 Value) = 0;
	virtual int32 GetPosition() const = 0;
};
