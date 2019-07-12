// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataField.h"

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"

#include <functional>

class UPsData;

/***********************************
 * FPsDataAllocator
 ***********************************/

typedef std::function<UPsData*()> FPsDataAllocator;

/***********************************
 * FPsDataSerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataSerializer
{
private:
	bool bUseSortedKeys;

public:
	FPsDataSerializer(bool InUseSortedKeys);

	virtual void WriteKey(const FString& Key) = 0;
	virtual void WriteArray() = 0;
	virtual void WriteObject() = 0;
	virtual void WriteValue(int32 Value) = 0;
	virtual void WriteValue(uint8 Value) = 0;
	virtual void WriteValue(float Value) = 0;
	virtual void WriteValue(bool Value) = 0;
	virtual void WriteValue(const FString& Value) = 0;
	virtual void WriteValue(const FName& Value) = 0;
	virtual void WriteValue(const UPsData* Value) = 0;

	virtual void PopKey(const FString& Key) = 0;
	virtual void PopArray() = 0;
	virtual void PopObject() = 0;

	bool UseSortedKeys();
};

/***********************************
 * FPsDataDeserializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataDeserializer
{
public:
	FPsDataDeserializer();

	virtual bool ReadKey(FString& OutKey) = 0;
	virtual bool ReadIndex() = 0;
	virtual bool ReadArray() = 0;
	virtual bool ReadObject() = 0;
	virtual bool ReadValue(int32& OutValue) = 0;
	virtual bool ReadValue(uint8& OutValue) = 0;
	virtual bool ReadValue(float& OutValue) = 0;
	virtual bool ReadValue(bool& OutValue) = 0;
	virtual bool ReadValue(FString& OutValue) = 0;
	virtual bool ReadValue(FName& OutValue) = 0;
	virtual bool ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator) = 0;

	virtual void PopKey(const FString& Key) = 0;
	virtual void PopIndex() = 0;
	virtual void PopArray() = 0;
	virtual void PopObject() = 0;
};
