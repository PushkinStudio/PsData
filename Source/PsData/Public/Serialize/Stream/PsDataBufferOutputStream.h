// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "Serialize/Stream/PsDataOutputStream.h"

#include "CoreMinimal.h"

/***********************************
 * FPsDataBufferOutputStream
 ***********************************/

struct PSDATA_API FPsDataBufferOutputStream : public FPsDataOutputStream
{
public:
	FPsDataBufferOutputStream();
	virtual ~FPsDataBufferOutputStream(){};

protected:
	TArray<uint8> Buffer;

public:
	TArray<uint8>& GetBuffer();

	void Reset();
	void Reserve(int32 NumBytes);

	virtual void WriteUint32(uint32 Value) override;
	virtual void WriteInt32(int32 Value) override;
	virtual void WriteUint64(uint64 Value) override;
	virtual void WriteInt64(int64 Value) override;
	virtual void WriteUint8(uint8 Value) override;
	virtual void WriteFloat(float Value) override;
	virtual void WriteBool(bool Value) override;
	virtual void WriteTCHAR(TCHAR Value) override;
	virtual void WriteString(const FString& Value) override;
	virtual void WriteBuffer(const TArray<uint8>& Value) override;
	virtual void WriteBuffer(const uint8* Value, int32 Count) override;
	virtual void WriteBuffer(TArray<uint8>&& Value) override;
	virtual int32 Size() const override;
};
