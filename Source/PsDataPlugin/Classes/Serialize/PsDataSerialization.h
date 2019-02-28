// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataField.h"

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * FPsDataSerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataSerializer
{
public:
	FPsDataSerializer();

	/***********************************
	 * int32
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, int32 Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data) = 0;

	/***********************************
	 * uint8
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, uint8 Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data) = 0;

	/***********************************
	 * float
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, float Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<float>& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data) = 0;

	/***********************************
	 * String
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const FString& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data) = 0;

	/***********************************
	 * bool
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, bool Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data) = 0;

	/***********************************
	 * Data
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, UPsData* Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data) = 0;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data) = 0;

	/***********************************
	* Custom
	***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data) = 0;
};

/***********************************
 * FPsDataDeserializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataDeserializer
{
public:
	FPsDataDeserializer();

	virtual bool Has(const TSharedPtr<const FDataField>& Field);

	/***********************************
	 * int32
	 ***********************************/

	virtual int32 Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, int32 Data, UClass* DataClass) = 0;
	virtual TArray<int32> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data, UClass* DataClass) = 0;
	virtual TMap<FString, int32> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data, UClass* DataClass) = 0;

	/***********************************
	 * uint8
	 ***********************************/

	virtual uint8 Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, uint8 Data, UClass* DataClass) = 0;
	virtual TArray<uint8> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data, UClass* DataClass) = 0;
	virtual TMap<FString, uint8> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data, UClass* DataClass) = 0;

	/***********************************
	 * float
	 ***********************************/

	virtual float Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, float Data, UClass* DataClass) = 0;
	virtual TArray<float> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<float>& Data, UClass* DataClass) = 0;
	virtual TMap<FString, float> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data, UClass* DataClass) = 0;

	/***********************************
	 * String
	 ***********************************/

	virtual FString Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Data, UClass* DataClass) = 0;
	virtual TArray<FString> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data, UClass* DataClass) = 0;
	virtual TMap<FString, FString> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data, UClass* DataClass) = 0;

	/***********************************
	 * bool
	 ***********************************/

	virtual bool Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, bool Data, UClass* DataClass) = 0;
	virtual TArray<bool> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data, UClass* DataClass) = 0;
	virtual TMap<FString, bool> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data, UClass* DataClass) = 0;

	/***********************************
	 * Data
	 ***********************************/

	virtual UPsData* Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, UPsData* Data, UClass* DataClass) = 0;
	virtual TArray<UPsData*> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data, UClass* DataClass) = 0;
	virtual TMap<FString, UPsData*> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data, UClass* DataClass) = 0;

	/***********************************
	 * Custom
	 ***********************************/

	virtual TSharedPtr<FJsonValue> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data, UClass* DataClass) = 0;
};
