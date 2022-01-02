// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

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
	TMap<FString, FString> KeyMap;
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

	uint8* GetRaw(UStruct* Struct)
	{
		return CreateStructFromJson(Struct, JsonSerializer.GetJson().ToSharedRef(), KeyMap);
	}

	template <typename T>
	T GetStruct()
	{
		T Result;
		void* Raw = GetRaw(T::StaticStruct());
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
	static uint8* CreateStructFromJson(UStruct* Struct, const TSharedRef<FJsonObject>& JsonObject, TMap<FString, FString>& KeyMap);

private:
	static void PropertyDeserialize(FProperty* Property, uint8* OutDest, const TSharedRef<FJsonValue>& JsonValue, TMap<FString, FString>& KeyMap);
	static void StructDeserialize(UStruct* Struct, uint8* OutDest, const TSharedRef<FJsonObject>& JsonObject, TMap<FString, FString>& KeyMap);

	static TSharedPtr<FJsonValue> FindJsonValueByProperty(const FProperty* Property, const TSharedRef<FJsonObject>& JsonObject, TMap<FString, FString>& KeyMap);
};

/***********************************
 * FPsDataStructDeserializer
 ***********************************/

struct PSDATA_API FPsDataStructDeserializer : public FPsDataDeserializer
{
private:
	TMap<FString, FString> KeyMap;
	FPsDataJsonDeserializer JsonDeserializer;

public:
	template <typename T>
	FPsDataStructDeserializer(const T& Struct)
		: FPsDataDeserializer()
		, JsonDeserializer(CreateJsonFromStruct(T::StaticStruct(), &Struct, KeyMap))
	{
	}

	FPsDataStructDeserializer(const UStruct* Struct, const void* Value)
		: FPsDataDeserializer()
		, JsonDeserializer(CreateJsonFromStruct(Struct, Value, KeyMap))
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
	static TSharedPtr<FJsonObject> CreateJsonFromStruct(const UStruct* Struct, const void* Value, TMap<FString, FString>& KeyMap);

private:
	static TSharedPtr<FJsonValue> PropertySerialize(FProperty* Property, const void* Value, TMap<FString, FString>& KeyMap);
	static TSharedPtr<FJsonValue> StructPropertySerialize(FStructProperty* StructProperty, const void* Value, TMap<FString, FString>& KeyMap);
	static TSharedPtr<FJsonValue> StructSerialize(const UStruct* Struct, const void* Value, TMap<FString, FString>& KeyMap);

public:
	static const FString& GetNormalizedKey(const FString& Key, TMap<FString, FString>& KeyMap);
};
