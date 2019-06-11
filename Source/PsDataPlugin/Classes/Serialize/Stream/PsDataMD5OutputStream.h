// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/Stream/PsDataOutputStream.h"

#include "Core/Public/Misc/SecureHash.h"
#include "CoreMinimal.h"

/***********************************
 * FPsDataMD5OutputStream
 ***********************************/

struct PSDATAPLUGIN_API FPsDataMD5OutputStream : public FPsDataOutputStream
{
public:
	FPsDataMD5OutputStream();

private:
	FMD5 Md5Gen;

public:
	FString GetHash();

	virtual void WriteInt32(int32 Value) override;
	virtual void WriteUint8(uint8 Value) override;
	virtual void WriteFloat(float Value) override;
	virtual void WriteBool(bool Value) override;
	virtual void WriteString(const FString& Value) override;
};
