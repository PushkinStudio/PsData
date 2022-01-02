// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "Serialize/PsDataBinarySerialization.h"
#include "Serialize/PsDataSerialization.h"
#include "Serialize/Stream/PsDataMD5OutputStream.h"
#include "Stream/PsDataBufferInputStream.h"
#include "Stream/PsDataBufferOutputStream.h"

#include "CoreMinimal.h"

class UPsData;

/***********************************
 * FPsDataImprintChild
 ***********************************/

struct FPsDataImprintChild
{
	FPsDataImprintChild(int32 InOffset, const UPsData* InData);

	int32 GetOffsets() const;
	const UPsData* GetData() const;

private:
	int32 Offset;
	const UPsData* Data;
};

/***********************************
 * FPsDataImprint
 ***********************************/

struct FPsDataImprint
{
	FPsDataImprint();

	void Prepare();

	void Reset();
	bool IsSet() const;

	TSharedRef<FPsDataBufferOutputStream> GetBufferOutputStream();
	const TArray<uint8>& GetBuffer() const;

	TArray<FPsDataImprintChild>& GetChildren();
	const TArray<FPsDataImprintChild>& GetChildren() const;
	int32 NumChildren() const;
	bool HasChildren() const;

	const FPsDataMD5Hash& GetHash() const;

private:
	bool bValid;
	TSharedRef<FPsDataBufferOutputStream> Buffer;
	TArray<FPsDataImprintChild> Children;
	TOptional<FPsDataMD5Hash> Hash;
};

/***********************************
 * FPsDataImprintBinarySerializer
 ***********************************/

struct FPsDataImprintBinarySerializer : public FPsDataBinarySerializer
{
	static uint32 Concatenate(TSharedRef<FPsDataBufferOutputStream>& OutputStream, const UPsData* Data);

	FPsDataImprintBinarySerializer(FPsDataImprint* InImprint);
	virtual ~FPsDataImprintBinarySerializer() override {}

	virtual void WriteValue(const UPsData* Value) override;

protected:
	FPsDataImprint* Imprint;
};

/***********************************
 * FPsDataImprintBinaryDeserializer
 ***********************************/

struct FPsDataImprintBinaryDeserializer : public FPsDataBinaryDeserializer
{
	FPsDataImprintBinaryDeserializer(TSharedRef<FPsDataBufferInputStream> InInputStream, int32 InOffset);
	virtual ~FPsDataImprintBinaryDeserializer() override {}

	virtual uint8 ReadToken() override;

protected:
	bool TryToRedirect(uint8 Token);

	TArray<int32> ReturnPositions;
};

/***********************************
 * FPsDataImprintBinaryConvertor
 ***********************************/

struct FPsDataImprintBinaryConvertor
{
	FPsDataImprintBinaryConvertor(FPsDataImprintBinaryDeserializer* InDeserializer);

	void Convert(FPsDataSerializer* Serializer);

protected:
	void ReadKey(FPsDataSerializer* Serializer);
	void ReadArray(FPsDataSerializer* Serializer);
	void ReadObject(FPsDataSerializer* Serializer);

	void PopKey(FPsDataSerializer* Serializer);
	void PopArray(FPsDataSerializer* Serializer);
	void PopObject(FPsDataSerializer* Serializer);

	void ReadNull(FPsDataSerializer* Serializer);

	template <typename T>
	void ReadValue(FPsDataSerializer* Serializer)
	{
		T Value;
		Deserializer->ReadValue(Value);
		Serializer->WriteValue(Value);
	}

	FPsDataImprintBinaryDeserializer* Deserializer;
	TArray<FString> Keys;
};
