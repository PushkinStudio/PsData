// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "UObject/ObjectMacros.h"

class UPsData;

/***********************************
 * FPsDataFastJsonSerializer
 ***********************************/

struct PSDATA_API FPsDataFastJsonSerializer : public FPsDataSerializer
{
public:
	FPsDataFastJsonSerializer(bool bPretty = false, int32 BufferSize = 1024 * 1024);

	virtual ~FPsDataFastJsonSerializer(){};

	FString& GetJsonString();

private:
	TArray<TCHAR> Buffer;
	FString JsonString;

	bool bPretty;
	int32 Depth;

	TSet<int32> CommaHelper;

	void AppendComma();
	void AppendSpace();
	void AppendValueSpace();

public:
	virtual void WriteKey(const FString& Key) override;
	virtual void WriteArray() override;
	virtual void WriteObject() override;
	virtual void WriteValue(int32 Value) override;
	virtual void WriteValue(int64 Value) override;
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

UENUM()
enum class EPsDataFastJsonToken : uint8
{
	Key = 1,
	Value = 2,
	OpenObject = 3,
	CloseObject = 4,
	OpenArray = 5,
	CloseArray = 6,
	Comma = 7,
};

/***********************************
 * FPsDataFastJsonLink
 ***********************************/

struct FPsDataFastJsonPointer
{
public:
	FPsDataFastJsonPointer(EPsDataFastJsonToken Token, int32 StartPosition, int32 EndPosition, int32 Depth);
	const FString& GetString(const TCHAR* Source);
	void Reset();

	EPsDataFastJsonToken Token;
	int32 Depth;

private:
	int32 StartPosition;
	int32 EndPosition;

	FString String;
	bool bHasString;
};

/***********************************
 * FPsDataFastJsonDeserializer
 ***********************************/

struct PSDATA_API FPsDataFastJsonDeserializer : public FPsDataDeserializer
{
public:
	FPsDataFastJsonDeserializer(const FString& InJsonString);

	virtual ~FPsDataFastJsonDeserializer(){};

private:
	const TCHAR* Source;
	int32 Size;
	TArray<FPsDataFastJsonPointer> Pointers;
	int32 PointerIndex;
	TArray<int32> DepthStack;

	void Parse();
	void SkipComma();

public:
	virtual bool ReadKey(FString& OutKey) override;
	virtual bool ReadArray() override;
	virtual bool ReadIndex() override;
	virtual bool ReadObject() override;
	virtual bool ReadValue(int32& OutValue) override;
	virtual bool ReadValue(int64& OutValue) override;
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
