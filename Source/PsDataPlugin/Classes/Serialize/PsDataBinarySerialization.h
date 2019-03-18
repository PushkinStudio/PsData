// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * FJsonDataSerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBinarySerializer : public FPsDataSerializer
{
private:
	TArray<char> Buffer;

public:
	FPsDataBinarySerializer();
	virtual ~FPsDataBinarySerializer(){};

	const TArray<char>& GetBuffer() const;

	virtual void WriteKey(const FString& Key) override;
	virtual void WriteArray() override;
	virtual void WriteObject() override;
	virtual void WriteValue(int32 Value) override;
	virtual void WriteValue(uint8 Value) override;
	virtual void WriteValue(float Value) override;
	virtual void WriteValue(bool Value) override;
	virtual void WriteValue(const FString& Value) override;
	virtual void WriteValue(const FName& Value) override;
	virtual void WriteValue(const UPsData* Value) override;

	virtual void PopKey(const FString& Key) override;
	virtual void PopArray() override;
	virtual void PopObject() override;
};
