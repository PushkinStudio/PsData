// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/Stream/PsDataOutputStream.h"

#include "CoreMinimal.h"

/***********************************
 * FPsDataBufferOutputStream
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBufferOutputStream : public FPsDataOutputStream
{
public:
	FPsDataBufferOutputStream();

private:
	TArray<char> Buffer;

public:
	const TArray<char>& GetBuffer();

	virtual void WriteInt32(int32 Value) override;
	virtual void WriteUint8(uint8 Value) override;
	virtual void WriteFloat(float Value) override;
	virtual void WriteBool(bool Value) override;
	virtual void WriteString(const FString& Value) override;
};
