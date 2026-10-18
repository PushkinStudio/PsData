// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsDataField.h"

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"

#include <functional>

class UPsData;

/***********************************
 * FPsDataAllocator
 ***********************************/

struct PSDATA_API FPsDataAllocator
{
	UClass* Class;
	UPsData* Parent;

	FPsDataAllocator(UClass* Class, UPsData* Parent);
	FPsDataAllocator(UField* Field, UPsData* Parent);
	FPsDataAllocator();
	UPsData* operator()() const;
};

/***********************************
 * FPsDataSerializer
 ***********************************/

struct PSDATA_API FPsDataSerializer
{
public:
	FPsDataSerializer();

	bool bWriteDefaults;

	virtual void WriteKey(const FString& Key) = 0;
	virtual void WriteArray() = 0;
	virtual void WriteObject() = 0;
	virtual void WriteValue(int32 Value) = 0;
	virtual void WriteValue(int64 Value) = 0;
	virtual void WriteValue(uint8 Value) = 0;
	virtual void WriteValue(float Value) = 0;
	virtual void WriteValue(bool Value) = 0;
	virtual void WriteValue(const FString& Value) = 0;
	virtual void WriteValue(const FName& Value) = 0;
	virtual void WriteValue(const UPsData* Value) = 0;

	/** Every built-in serializer writes doubles exactly; the fallback for others narrows to float */
	virtual void WriteValue(double Value);

	virtual void PopKey(const FString& Key) = 0;
	virtual void PopArray() = 0;
	virtual void PopObject() = 0;
};

/***********************************
 * EPsDataDeserializedValueType
 ***********************************/

/** What a deserializer holds next, so a document can be copied without knowing its schema */
enum class EPsDataDeserializedValueType : uint8
{
	None = 0,
	Null,
	Object,
	Array,
	Int32,
	Int64,
	Uint8,
	Float,
	Double,
	Boolean,
	String,
};

/***********************************
 * FPsDataDeserializer
 ***********************************/

struct PSDATA_API FPsDataDeserializer
{
public:
	FPsDataDeserializer();

	virtual bool ReadKey(FString& OutKey) = 0;
	virtual bool ReadIndex() = 0;
	virtual bool ReadArray() = 0;
	virtual bool ReadObject() = 0;
	virtual bool ReadValue(int32& OutValue) = 0;
	virtual bool ReadValue(int64& Value) = 0;
	virtual bool ReadValue(uint8& OutValue) = 0;
	virtual bool ReadValue(float& OutValue) = 0;
	virtual bool ReadValue(bool& OutValue) = 0;
	virtual bool ReadValue(FString& OutValue) = 0;
	virtual bool ReadValue(FName& OutValue) = 0;
	virtual bool ReadValue(UPsData*& OutValue, FPsDataAllocator Allocator) = 0;

	/** Every built-in deserializer reads doubles exactly; the fallback for others reads a float */
	virtual bool ReadValue(double& OutValue);

	virtual void PopKey(const FString& Key) = 0;
	virtual void PopIndex() = 0;
	virtual void PopArray() = 0;
	virtual void PopObject() = 0;

	/** The type of the value at the read position, without consuming it. None when unknown or at the end */
	virtual EPsDataDeserializedValueType NextValueType();

	/** Copies the next value, with everything nested in it, into Serializer. False if the value could not be read */
	bool Translate(FPsDataSerializer* Serializer);
};
