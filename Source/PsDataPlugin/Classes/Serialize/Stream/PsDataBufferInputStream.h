// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/Stream/PsDataInputStream.h"

#include "CoreMinimal.h"

/***********************************
 * FPsDataBufferInputStream
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBufferInputStream : public FPsDataInputStream
{
public:
	FPsDataBufferInputStream(const TArray<uint8>& InBuffer);

private:
	const TArray<uint8>& Buffer;
	int32 Index;
	int32 PrevIndex;

public:
	virtual uint32 ReadUint32() override;
	virtual int32 ReadInt32() override;
	virtual uint8 ReadUint8() override;
	virtual float ReadFloat() override;
	virtual bool ReadBool() override;
	virtual TCHAR ReadTCHAR() override;
	virtual FString ReadString() override;
	virtual bool HasData() override;
	virtual void ShiftBack() override;

protected:
	void CheckRange();
};
