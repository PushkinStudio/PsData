// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Collection/PsDataMapProxy.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/DataTable.h"
#include "Internationalization/Regex.h"

class UPsData;

/***********************************
 * FTableDataSerializer
 ***********************************/

struct PSDATAPLUGIN_API FPsDataTableDeserializer : public FPsDataDeserializer
{
protected:
	TWeakObjectPtr<UDataTable> DataTablePtr;

	FString PropertyName;

public:
	FPsDataTableDeserializer(UDataTable* DataTable, const FString& PropertyName);

	template <typename T, bool bConst>
	FPsDataTableDeserializer(UDataTable* DataTable, const FPsDataBaseMapProxy<T, bConst>& MapProxy)
		: FPsDataTableDeserializer(DataTable, MapProxy.GetField()->Name)
	{
	}

	virtual ~FPsDataTableDeserializer(){};

public:
	virtual bool Has(const TSharedPtr<const FDataField>& Field) override;

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

protected:
	/***********************************
	* Struct serialize
	***********************************/

	FRegexPattern PropertyPattern;

	TMap<uint32, TMap<FString, FString>> PropertyMap;

	TMap<FString, FString>& UpdatePropertyMap(const UStruct* Struct, TMap<FString, TSharedPtr<FJsonValue>>& Values);
	void CompareTableAndClass(const UStruct* Struct, UClass* DataClass);

	TSharedPtr<FJsonValue> PropertySerialize(UProperty* Property, const void* Value);
	TSharedPtr<FJsonValue> StructPropertySerialize(UStructProperty* StructProperty, const void* Value);
	TSharedPtr<FJsonValue> StructSerialize(const UStruct* Struct, const void* Value);
};
