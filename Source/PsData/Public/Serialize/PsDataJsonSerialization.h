// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * FPsDataJsonSerializer
 ***********************************/

struct PSDATA_API FPsDataJsonSerializer : public FPsDataSerializer
{
private:
	TSharedPtr<FJsonObject> RootJson;
	TArray<TSharedPtr<FJsonValue>> Values;
	TArray<FString> Keys;

public:
	FPsDataJsonSerializer(TSharedPtr<FJsonObject> InRootJson);
	FPsDataJsonSerializer();
	virtual ~FPsDataJsonSerializer(){};

	TSharedPtr<FJsonObject> GetJson();

private:
	void WriteJsonValue(TSharedPtr<FJsonValue> Value);

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

/***********************************
 * FPsDataJsonDeserializer
 ***********************************/

struct PSDATA_API FPsDataJsonDeserializer : public FPsDataDeserializer
{
private:
	TSharedPtr<FJsonObject> RootJson;
	TSharedPtr<FJsonValueObject> RootValue;
	TArray<TSharedPtr<FJsonValue>> Values;
	TMap<TSharedPtr<FJsonValue>, TMap<FString, TSharedPtr<FJsonValue>>::TIterator> KeysIterator;
	TMap<TSharedPtr<FJsonValue>, TArray<TSharedPtr<FJsonValue>>::TIterator> IndicesIterator;

#if WITH_EDITORONLY_DATA
	TSet<TSharedPtr<FJsonValue>> Used;
#endif

public:
	FPsDataJsonDeserializer(TSharedPtr<FJsonObject> InJson);
	virtual ~FPsDataJsonDeserializer(){};

	TSharedPtr<FJsonObject>& GetJson();

private:
	TSharedPtr<FJsonValue> ReadJsonValue() const;

public:
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
