// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/***********************************
 * FPsDataOutputStream
 ***********************************/

struct PSDATAPLUGIN_API FPsDataOutputStream
{
	virtual void WriteUint32(uint32 Value) = 0;
	virtual void WriteInt32(int32 Value) = 0;
	virtual void WriteUint8(uint8 Value) = 0;
	virtual void WriteFloat(float Value) = 0;
	virtual void WriteBool(bool Value) = 0;
	virtual void WriteTCHAR(TCHAR Value) = 0;
	virtual void WriteString(const FString& Value) = 0;
};
