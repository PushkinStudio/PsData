// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/PsDataSerialization.h"
#include "Serialize/Stream/PsDataInputStream.h"
#include "Serialize/Stream/PsDataOutputStream.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * Tokens for binary serializer/deserializer
 ***********************************/

enum class EBinaryTokens : uint8
{
	Key = 1,
	ArrayBegin = 2,
	ArrayEnd = 3,
	ObjectBegin = 4,
	ObjectEnd = 5,
	Value_int32 = 6,
	Value_uint8 = 7,
	Value_float = 8,
	Value_bool = 9,
	Value_FString = 10,
	Value_FName = 11,
	Value_null = 12,
};

/***********************************
 * FPsDataBinarySerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBinarySerializer : public FPsDataSerializer
{
private:
	TSharedRef<FPsDataOutputStream> OutputStream;

public:
	FPsDataBinarySerializer(TSharedRef<FPsDataOutputStream> InOutputStream, bool bUseSortedKeys);
	virtual ~FPsDataBinarySerializer(){};

	TSharedRef<FPsDataOutputStream> GetOutputStream() const;

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

/***********************************
 * FPsDataBinaryDeserializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBinaryDeserializer : public FPsDataDeserializer
{
private:
	TSharedRef<FPsDataInputStream> InputStream;

public:
	FPsDataBinaryDeserializer(TSharedRef<FPsDataInputStream> InInputStream);
	virtual ~FPsDataBinaryDeserializer(){};

private:
	bool ReadToken(EBinaryTokens Token);

public:
	virtual bool ReadKey(FString& OutKey) override;
	virtual bool ReadIndex() override;
	virtual bool ReadArray() override;
	virtual bool ReadObject() override;
	virtual bool ReadValue(int32& OutValue) override;
	virtual bool ReadValue(uint8& OutValue) override;
	virtual bool ReadValue(float& OutValue) override;
	virtual bool ReadValue(bool& OutValue) override;
	virtual bool ReadValue(FString& OutValue) override;
	virtual bool ReadValue(FName& OutValue) override;
	virtual bool ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator) override;

	virtual void PopKey(const FString& Key) override;
	virtual void PopIndex() override;
	virtual void PopArray() override;
	virtual void PopObject() override;
};
