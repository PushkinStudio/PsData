// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class UPsData;

/***********************************
 * FPsDataJsonSerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataJsonSerializer : public FPsDataSerializer
{
protected:
	TSharedPtr<FJsonObject> RootJson;
	TArray<TSharedPtr<FJsonObject>> JsonPath;

public:
	FPsDataJsonSerializer(TSharedPtr<FJsonObject> InJson);
	FPsDataJsonSerializer();
	virtual ~FPsDataJsonSerializer(){};

protected:
	void PushJson(TSharedPtr<FJsonObject> Json);
	void PopJson(TSharedPtr<FJsonObject> Json);
	TSharedPtr<FJsonObject> GetLastJson();

public:
	TSharedPtr<FJsonObject>& GetJson();

	/***********************************
	 * int32
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, int32 Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data) override;

	/***********************************
	 * uint8
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, uint8 Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data) override;

	/***********************************
	 * float
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, float Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<float>& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data) override;

	/***********************************
	 * String
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const FString& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data) override;

	/***********************************
	 * bool
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, bool Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data) override;

	/***********************************
	 * Data
	 ***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, UPsData* Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data) override;
	virtual void Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data) override;

	/***********************************
	* Custom
	***********************************/

	virtual void Serialize(const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data) override;
};

/***********************************
 * FPsDataJsonDeserializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataJsonDeserializer : public FPsDataDeserializer
{
protected:
	TSharedPtr<FJsonObject> RootJson;
	TArray<TSharedPtr<FJsonObject>> JsonPath;

public:
	FPsDataJsonDeserializer(TSharedPtr<FJsonObject> InJson);
	virtual ~FPsDataJsonDeserializer(){};

protected:
	void PushJson(TSharedPtr<FJsonObject> Json);
	void PopJson(TSharedPtr<FJsonObject> Json);
	TSharedPtr<FJsonObject> GetLastJson();
	FString GetStringType(EJson Type);

public:
	virtual bool Has(const TSharedPtr<const FDataField>& Field) override;
	TSharedPtr<FJsonObject>& GetJson();

	/***********************************
	 * int32
	 ***********************************/

	virtual int32 Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, int32 Data, UClass* DataClass) override;
	virtual TArray<int32> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data, UClass* DataClass) override;
	virtual TMap<FString, int32> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data, UClass* DataClass) override;

	/***********************************
	 * uint8
	 ***********************************/

	virtual uint8 Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, uint8 Data, UClass* DataClass) override;
	virtual TArray<uint8> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data, UClass* DataClass) override;
	virtual TMap<FString, uint8> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data, UClass* DataClass) override;

	/***********************************
	 * float
	 ***********************************/

	virtual float Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, float Data, UClass* DataClass) override;
	virtual TArray<float> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<float>& Data, UClass* DataClass) override;
	virtual TMap<FString, float> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data, UClass* DataClass) override;

	/***********************************
	 * String
	 ***********************************/

	virtual FString Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Data, UClass* DataClass) override;
	virtual TArray<FString> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data, UClass* DataClass) override;
	virtual TMap<FString, FString> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data, UClass* DataClass) override;

	/***********************************
	 * bool
	 ***********************************/

	virtual bool Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, bool Data, UClass* DataClass) override;
	virtual TArray<bool> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data, UClass* DataClass) override;
	virtual TMap<FString, bool> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data, UClass* DataClass) override;

	/***********************************
	 * Data
	 ***********************************/

	virtual UPsData* Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, UPsData* Data, UClass* DataClass) override;
	virtual TArray<UPsData*> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data, UClass* DataClass) override;
	virtual TMap<FString, UPsData*> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data, UClass* DataClass) override;

	/***********************************
	 * Custom
	 ***********************************/

	virtual TSharedPtr<FJsonValue> Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data, UClass* DataClass) override;
};
