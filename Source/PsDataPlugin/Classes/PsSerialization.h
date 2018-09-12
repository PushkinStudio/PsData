// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"

class UPsData;

struct PSDATAPLUGIN_API FDataSerializer
{
protected:
	bool bSerialize;
	
public:
	FDataSerializer(bool bInSerialize);
	
	virtual ~FDataSerializer() {}
	
	virtual bool IsSerialize();
	virtual bool Has(const FString& Key);
	
	/***********************************
	 * Data
	 ***********************************/
	
	virtual void Serialize(const FString& Key, UPsData* Data);
	virtual void Serialize(const FString& Key, const TArray<UPsData*>& Data);
	virtual void Serialize(const FString& Key, const TMap<FString, UPsData*>& Data);
	
	virtual UPsData* Deserialize(UPsData* Instance, const FString& Key, UPsData* Data, UClass* DataClass);
	virtual TArray<UPsData*> Deserialize(UPsData* Instance, const FString& Key, const TArray<UPsData*>& Data, UClass* DataClass);
	virtual TMap<FString, UPsData*> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, UPsData*>& Data, UClass* DataClass);
	
	/***********************************
	 * Int
	 ***********************************/
	
	virtual void Serialize(const FString& Key, int32 Data);
	virtual void Serialize(const FString& Key, const TArray<int32>& Data);
	virtual void Serialize(const FString& Key, const TMap<FString, int32>& Data);
	
	virtual int32 Deserialize(UPsData* Instance, const FString& Key, int32 Data, UClass* DataClass);
	virtual TArray<int32> Deserialize(UPsData* Instance, const FString& Key, const TArray<int32>& Data, UClass* DataClass);
	virtual TMap<FString, int32> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, int32>& Data, UClass* DataClass);
	
	/***********************************
	 * Float
	 ***********************************/
	
	virtual void Serialize(const FString& Key, float Data);
	virtual void Serialize(const FString& Key, const TArray<float>& Data);
	virtual void Serialize(const FString& Key, const TMap<FString, float>& Data);
	
	virtual float Deserialize(UPsData* Instance, const FString& Key, float Data, UClass* DataClass);
	virtual TArray<float> Deserialize(UPsData* Instance, const FString& Key, const TArray<float>& Data, UClass* DataClass);
	virtual TMap<FString, float> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, float>& Data, UClass* DataClass);
	
	/***********************************
	 * String
	 ***********************************/
	
	virtual void Serialize(const FString& Key, const FString& Data);
	virtual void Serialize(const FString& Key, const TArray<FString>& Data);
	virtual void Serialize(const FString& Key, const TMap<FString, FString>& Data);
	
	virtual FString Deserialize(UPsData* Instance, const FString& Key, const FString& Data, UClass* DataClass);
	virtual TArray<FString> Deserialize(UPsData* Instance, const FString& Key, const TArray<FString>& Data, UClass* DataClass);
	virtual TMap<FString, FString> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, FString>& Data, UClass* DataClass);
	
	/***********************************
	 * Bool
	 ***********************************/
	
	virtual void Serialize(const FString& Key, bool Data);
	virtual void Serialize(const FString& Key, const TArray<bool>& Data);
	virtual void Serialize(const FString& Key, const TMap<FString, bool>& Data);
	
	virtual bool Deserialize(UPsData* Instance, const FString& Key, bool Data, UClass* DataClass);
	virtual TArray<bool> Deserialize(UPsData* Instance, const FString& Key, const TArray<bool>& Data, UClass* DataClass);
	virtual TMap<FString, bool> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, bool>& Data, UClass* DataClass);
};

struct PSDATAPLUGIN_API FJsonDataSerializer : public FDataSerializer
{
protected:
	TSharedPtr<FJsonObject> RootJson;
	TArray<TSharedPtr<FJsonObject>> JsonPath;
	
public:
	FJsonDataSerializer(TSharedPtr<FJsonObject> InJson, bool bInSerialize);
	
	FJsonDataSerializer(TSharedPtr<FJsonObject> InJson);
	
	FJsonDataSerializer();
	
	virtual ~FJsonDataSerializer() {}
	
protected:
	void PushJson(TSharedPtr<FJsonObject> Json);
	void PopJson(TSharedPtr<FJsonObject> Json);
	TSharedPtr<FJsonObject> GetLastJson();
	FString GetStringType(EJson Type);
	
public:
	virtual bool Has(const FString& Key) override;
	TSharedPtr<FJsonObject>& GetJson();
	
	/***********************************
	 * Data
	 ***********************************/
	
	virtual void Serialize(const FString& Key, UPsData* Data) override;
	virtual void Serialize(const FString& Key, const TArray<UPsData*>& Data) override;
	virtual void Serialize(const FString& Key, const TMap<FString, UPsData*>& Data) override;
	
	virtual UPsData* Deserialize(UPsData* Instance, const FString& Key, UPsData* Data, UClass* DataClass) override;
	virtual TArray<UPsData*> Deserialize(UPsData* Instance, const FString& Key, const TArray<UPsData*>& Data, UClass* DataClass) override;
	virtual TMap<FString, UPsData*> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, UPsData*>& Data, UClass* DataClass) override;
	
	/***********************************
	 * Int
	 ***********************************/
	
	virtual void Serialize(const FString& Key, int32 Data) override;
	virtual void Serialize(const FString& Key, const TArray<int32>& Data) override;
	virtual void Serialize(const FString& Key, const TMap<FString, int32>& Data) override;
	
	virtual int32 Deserialize(UPsData* Instance, const FString& Key, int32 Data, UClass* DataClass) override;
	virtual TArray<int32> Deserialize(UPsData* Instance, const FString& Key, const TArray<int32>& Data, UClass* DataClass) override;
	virtual TMap<FString, int32> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, int32>& Data, UClass* DataClass) override;
	
	/***********************************
	 * Float
	 ***********************************/
	
	virtual void Serialize(const FString& Key, float Data) override;
	virtual void Serialize(const FString& Key, const TArray<float>& Data) override;
	virtual void Serialize(const FString& Key, const TMap<FString, float>& Data) override;
	
	virtual float Deserialize(UPsData* Instance, const FString& Key, float Data, UClass* DataClass) override;
	virtual TArray<float> Deserialize(UPsData* Instance, const FString& Key, const TArray<float>& Data, UClass* DataClass) override;
	virtual TMap<FString, float> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, float>& Data, UClass* DataClass) override;
	
	/***********************************
	 * String
	 ***********************************/
	
	virtual void Serialize(const FString& Key, const FString& Data) override;
	virtual void Serialize(const FString& Key, const TArray<FString>& Data) override;
	virtual void Serialize(const FString& Key, const TMap<FString, FString>& Data) override;
	
	virtual FString Deserialize(UPsData* Instance, const FString& Key, const FString& Data, UClass* DataClass) override;
	virtual TArray<FString> Deserialize(UPsData* Instance, const FString& Key, const TArray<FString>& Data, UClass* DataClass) override;
	virtual TMap<FString, FString> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, FString>& Data, UClass* DataClass) override;
	
	/***********************************
	 * Bool
	 ***********************************/
	
	virtual void Serialize(const FString& Key, bool Data) override;
	virtual void Serialize(const FString& Key, const TArray<bool>& Data) override;
	virtual void Serialize(const FString& Key, const TMap<FString, bool>& Data) override;
	
	virtual bool Deserialize(UPsData* Instance, const FString& Key, bool Data, UClass* DataClass) override;
	virtual TArray<bool> Deserialize(UPsData* Instance, const FString& Key, const TArray<bool>& Data, UClass* DataClass) override;
	virtual TMap<FString, bool> Deserialize(UPsData* Instance, const FString& Key, const TMap<FString, bool>& Data, UClass* DataClass) override;
};
