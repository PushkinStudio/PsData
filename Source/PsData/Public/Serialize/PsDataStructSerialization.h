// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * FPsDataStructSerializer
 ***********************************/

struct PSDATA_API FPsDataStructSerializer : public FPsDataSerializer
{
private:
	FPsDataJsonSerializer JsonSerializer;

public:
	FPsDataStructSerializer(TSharedPtr<FJsonObject> InRootJson)
		: FPsDataSerializer()
		, JsonSerializer(InRootJson)
	{
		JsonSerializer.bWriteDefaults = true;
		bWriteDefaults = true;
	}

	FPsDataStructSerializer()
		: FPsDataSerializer()
	{
		JsonSerializer.bWriteDefaults = true;
		bWriteDefaults = true;
	}

	uint8* GetRaw(UStruct* Struct, bool bInitialize = true)
	{
		return CreateStructFromJson(Struct, JsonSerializer.GetJson().ToSharedRef(), bInitialize);
	}

	template <typename T>
	T GetStruct(bool bInitialize = true)
	{
		T Result;
		void* Raw = GetRaw(T::StaticStruct(), bInitialize);
		Result = *static_cast<T*>(Raw);
		FMemory::Free(Raw);
		return Result;
	}

	virtual ~FPsDataStructSerializer(){};

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

	/***********************************
	 * Struct deserialize
	 ***********************************/
public:
	static uint8* CreateStructFromJson(const UStruct* Struct, const TSharedRef<FJsonObject>& JsonObject, bool bInitialize = true);
	static uint8* CreateStructFromJson_Import(const UStruct* Struct, const TSharedRef<FJsonObject>& JsonObject, TArray<FString>& ImportProblems);

private:
	static void PropertyDeserialize(FProperty* Property, uint8* OutDest, const TSharedRef<FJsonValue>& JsonValue);
	static void StructDeserialize(const UStruct* Struct, uint8* OutDest, const TSharedRef<FJsonObject>& JsonObject, bool bInitialize);

	static TSharedPtr<FJsonValue> FindJsonValueByProperty(const FProperty* Property, const TSharedRef<FJsonObject>& JsonObject);
};

/***********************************
 * FPsDataStructDeserializer
 ***********************************/

struct PSDATA_API FPsDataStructDeserializer : public FPsDataDeserializer
{
private:
	FPsDataJsonDeserializer JsonDeserializer;

public:
	template <typename T>
	FPsDataStructDeserializer(const T& Struct)
		: FPsDataDeserializer()
		, JsonDeserializer(CreateJsonFromStruct(T::StaticStruct(), &Struct))
	{
	}

	FPsDataStructDeserializer(const UStruct* Struct, const void* Value)
		: FPsDataDeserializer()
		, JsonDeserializer(CreateJsonFromStruct(Struct, Value))
	{
	}

	virtual ~FPsDataStructDeserializer(){};

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

	/***********************************
	 * Struct serialize
	 ***********************************/
public:
	static TSharedPtr<FJsonObject> CreateJsonFromStruct(const UStruct* Struct, const void* Value);
	static TSharedPtr<FJsonObject> CreateJsonFromStruct_Export(const UStruct* Struct, const void* Value);

private:
	static TSharedPtr<FJsonValue> PropertySerialize(FProperty* Property, const void* Value);
	static TSharedPtr<FJsonValue> StructPropertySerialize(FStructProperty* StructProperty, const void* Value);
	static TSharedPtr<FJsonValue> StructSerialize(const UStruct* Struct, const void* Value);
};
