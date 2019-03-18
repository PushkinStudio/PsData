// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * FTableDataSerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataStructDeserializer : public FPsDataDeserializer
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

	virtual ~FPsDataStructDeserializer(){};

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

	/***********************************
	 * Struct serialize
	 ***********************************/
public:
	static TSharedPtr<FJsonObject> CreateJsonFromStruct(const UStruct* Struct, const void* Value);

private:
	static TSharedPtr<FJsonValue> PropertySerialize(UProperty* Property, const void* Value);
	static TSharedPtr<FJsonValue> StructPropertySerialize(UStructProperty* StructProperty, const void* Value);
	static TSharedPtr<FJsonValue> StructSerialize(const UStruct* Struct, const void* Value);

	static FString GetNormalizedKey(const FString& Key);
};
