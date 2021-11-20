// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/PsDataSerialization.h"
#include "Serialize/Stream/PsDataInputStream.h"
#include "Serialize/Stream/PsDataOutputStream.h"

#include "CoreMinimal.h"

class UPsData;

/***********************************
 * Tokens for binary serializer/deserializer
 ***********************************/

namespace EBinaryTokens
{
constexpr uint8 Null = 0; // 0

constexpr uint8 KeyBegin = '$'; // 36
constexpr uint8 KeyEnd = '%';   // 37

constexpr uint8 ArrayBegin = '['; // 91
constexpr uint8 ArrayEnd = ']';   // 93

constexpr uint8 ObjectBegin = '{'; // 123
constexpr uint8 ObjectEnd = '}';   // 125

constexpr uint8 Value_uint8 = 'A';  // 65
constexpr uint8 Value_int8 = 'B';   // 66
constexpr uint8 Value_uint16 = 'C'; // 67
constexpr uint8 Value_int16 = 'D';  // 68
constexpr uint8 Value_uint32 = 'E'; // 69
constexpr uint8 Value_int32 = 'F';  // 70
constexpr uint8 Value_uint64 = 'G'; // 71
constexpr uint8 Value_int64 = 'H';  // 72
constexpr uint8 Value_float = 'I';  // 73
constexpr uint8 Value_double = 'J'; // 74

constexpr uint8 Value_null = 'a';    // 97
constexpr uint8 Value_bool = 'b';    // 98
constexpr uint8 Value_FString = 'c'; // 99
constexpr uint8 Value_FName = 'd';   // 100

constexpr uint8 Redirect = 26;    // 26
constexpr uint8 RedirectEnd = 10; // 10
} // namespace EBinaryTokens

/***********************************
 * FPsDataBinarySerializer
 ***********************************/

struct PSDATA_API FPsDataBinarySerializer : public FPsDataSerializer
{
protected:
	TSharedRef<FPsDataOutputStream> OutputStream;

public:
	FPsDataBinarySerializer(TSharedRef<FPsDataOutputStream> InOutputStream);
	virtual ~FPsDataBinarySerializer() {}

	TSharedRef<FPsDataOutputStream> GetOutputStream() const;

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

/***********************************
 * FPsDataBinaryDeserializer
 ***********************************/

struct PSDATA_API FPsDataBinaryDeserializer : public FPsDataDeserializer
{
protected:
	TSharedRef<FPsDataInputStream> InputStream;

public:
	FPsDataBinaryDeserializer(TSharedRef<FPsDataInputStream> InInputStream);
	virtual ~FPsDataBinaryDeserializer() {}

	TSharedRef<FPsDataInputStream> GetInputStream() const;

	virtual uint8 ReadToken();
	bool CheckToken(uint8 Token);

	virtual bool ReadKey(FString& OutKey) override;
	virtual bool ReadIndex() override;
	virtual bool ReadArray() override;
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
