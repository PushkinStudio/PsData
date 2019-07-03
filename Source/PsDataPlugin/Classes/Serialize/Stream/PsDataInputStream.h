// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/***********************************
 * FPsDataInputStream
 ***********************************/

struct PSDATAPLUGIN_API FPsDataInputStream
{
	virtual int32 ReadInt32() = 0;
	virtual uint8 ReadUint8() = 0;
	virtual float ReadFloat() = 0;
	virtual bool ReadBool() = 0;
	virtual FString ReadString() = 0;
	virtual bool HasData() = 0;
	virtual void ShiftBack() = 0;
};
