// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/TextProperty.h"

#include "PsDataFunctionLibrary.generated.h"

class UPsData;
class UPsDataBlueprintMapProxy;
class UPsDataBlueprintArrayProxy;
struct FDataLink;

UCLASS()
class PSDATAPLUGIN_API UPsDataFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/***********************************
	 * int32
	 ***********************************/

	/** Get int property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static int32 GetIntProperty(UPsData* Target, int32 Crc32);

	/** Set int property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetIntProperty(UPsData* Target, int32 Crc32, int32 Value);

	/** Get int array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<int32>& GetIntArrayProperty(UPsData* Target, int32 Crc32);

	/** Set int array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetIntArrayProperty(UPsData* Target, int32 Crc32, const TArray<int32>& Value);

	/** Get int map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, int32>& GetIntMapProperty(UPsData* Target, int32 Crc32);

	/** Set int map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetIntMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, int32>& Value);

	/***********************************
	 * uint8
	 ***********************************/

	/** Get uint8 property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static uint8 GetByteProperty(UPsData* Target, int32 Crc32);

	/** Set uint8 property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetByteProperty(UPsData* Target, int32 Crc32, uint8 Value);

	/** Get uint8 array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<uint8>& GetByteArrayProperty(UPsData* Target, int32 Crc32);

	/** Set uint8 array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetByteArrayProperty(UPsData* Target, int32 Crc32, const TArray<uint8>& Value);

	/** Get uint8 map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, uint8>& GetByteMapProperty(UPsData* Target, int32 Crc32);

	/** Set uint8 map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetByteMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, uint8>& Value);

	/***********************************
	 * float
	 ***********************************/

	/** Get float property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static float GetFloatProperty(UPsData* Target, int32 Crc32);

	/** Set float property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetFloatProperty(UPsData* Target, int32 Crc32, float Value);

	/** Get float array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<float>& GetFloatArrayProperty(UPsData* Target, int32 Crc32);

	/** Set float array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetFloatArrayProperty(UPsData* Target, int32 Crc32, const TArray<float>& Value);

	/** Get float map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, float>& GetFloatMapProperty(UPsData* Target, int32 Crc32);

	/** Set float map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetFloatMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, float>& Value);

	/***********************************
	 * bool
	 ***********************************/

	/** Get bool property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static bool GetBoolProperty(UPsData* Target, int32 Crc32);

	/** Set bool property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetBoolProperty(UPsData* Target, int32 Crc32, bool Value);

	/** Get bool array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<bool>& GetBoolArrayProperty(UPsData* Target, int32 Crc32);

	/** Set bool array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetBoolArrayProperty(UPsData* Target, int32 Crc32, const TArray<bool>& Value);

	/** Get bool map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, bool>& GetBoolMapProperty(UPsData* Target, int32 Crc32);

	/** Set bool map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetBoolMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, bool>& Value);

	/***********************************
	 * String
	 ***********************************/

	/** Get string property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static FString GetStringProperty(UPsData* Target, int32 Crc32);

	/** Set string property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetStringProperty(UPsData* Target, int32 Crc32, const FString& Value);

	/** Get string array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<FString>& GetStringArrayProperty(UPsData* Target, int32 Crc32);

	/** Set string array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetStringArrayProperty(UPsData* Target, int32 Crc32, const TArray<FString>& Value);

	/** Get string map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, FString>& GetStringMapProperty(UPsData* Target, int32 Crc32);

	/** Set string map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetStringMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FString>& Value);

	/***********************************
	 * FText
	 ***********************************/

	/** Get text property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static FText GetTextProperty(UPsData* Target, int32 Crc32);

	/** Set text property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetTextProperty(UPsData* Target, int32 Crc32, const FText& Value);

	/** Get text array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<FText>& GetTextArrayProperty(UPsData* Target, int32 Crc32);

	/** Set text array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetTextArrayProperty(UPsData* Target, int32 Crc32, const TArray<FText>& Value);

	/** Get text map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, FText>& GetTextMapProperty(UPsData* Target, int32 Crc32);

	/** Set text map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetTextMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FText>& Value);

	/***********************************
	 * FName
	 ***********************************/

	/** Get name property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static FName GetNameProperty(UPsData* Target, int32 Crc32);

	/** Set name property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetNameProperty(UPsData* Target, int32 Crc32, const FName& Value);

	/** Get name array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<FName>& GetNameArrayProperty(UPsData* Target, int32 Crc32);

	/** Set name array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetNameArrayProperty(UPsData* Target, int32 Crc32, const TArray<FName>& Value);

	/** Get name map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, FName>& GetNameMapProperty(UPsData* Target, int32 Crc32);

	/** Set name map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetNameMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FName>& Value);

	/***********************************
	 * Data
	 ***********************************/

	/** Get data property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static UPsData* GetDataProperty(UPsData* Target, int32 Crc32);

	/** Set data property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetDataProperty(UPsData* Target, int32 Crc32, UPsData* Value);

	/** Get data array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<UPsData*>& GetDataArrayProperty(UPsData* Target, int32 Crc32);

	/** Set data array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetDataArrayProperty(UPsData* Target, int32 Crc32, const TArray<UPsData*>& Value);

	/** Get data map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, UPsData*>& GetDataMapProperty(UPsData* Target, int32 Crc32);

	/** Set data map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetDataMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, UPsData*>& Value);

	/***********************************
	 * TSoftObject
	 ***********************************/

	/** Get soft object property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TSoftObjectPtr<UObject>& GetSoftObjectProperty(UPsData* Target, int32 Crc32);

	/** Set soft object property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetSoftObjectProperty(UPsData* Target, int32 Crc32, const TSoftObjectPtr<UObject>& Value);

	/** Get soft object array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<TSoftObjectPtr<UObject>>& GetSoftObjectArrayProperty(UPsData* Target, int32 Crc32);

	/** Set soft object array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetSoftObjectArrayProperty(UPsData* Target, int32 Crc32, const TArray<TSoftObjectPtr<UObject>>& Value);

	/** Get soft object map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, TSoftObjectPtr<UObject>>& GetSoftObjectMapProperty(UPsData* Target, int32 Crc32);

	/** Set soft object map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetSoftObjectMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, TSoftObjectPtr<UObject>>& Value);

	/***********************************
	 * TSoftClass
	 ***********************************/

	/** Get soft class property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TSoftClassPtr<UObject>& GetSoftClassProperty(UPsData* Target, int32 Crc32);

	/** Set soft class property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetSoftClassProperty(UPsData* Target, int32 Crc32, const TSoftClassPtr<UObject>& Value);

	/** Get soft class array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<TSoftClassPtr<UObject>>& GetSoftClassArrayProperty(UPsData* Target, int32 Crc32);

	/** Set soft class array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetSoftClassArrayProperty(UPsData* Target, int32 Crc32, const TArray<TSoftClassPtr<UObject>>& Value);

	/** Get soft class map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, TSoftClassPtr<UObject>>& GetSoftClassMapProperty(UPsData* Target, int32 Crc32);

	/** Set soft class map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetSoftClassMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, TSoftClassPtr<UObject>>& Value);

	/***********************************
	 * FLinearColor
	 ***********************************/

	/** Get linear color property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static FLinearColor GetLinearColorProperty(UPsData* Target, int32 Crc32);

	/** Set linear color property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetLinearColorProperty(UPsData* Target, int32 Crc32, const FLinearColor& Value);

	/** Get linear color array property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TArray<FLinearColor>& GetLinearColorArrayProperty(UPsData* Target, int32 Crc32);

	/** Set linear color array property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetLinearColorArrayProperty(UPsData* Target, int32 Crc32, const TArray<FLinearColor>& Value);

	/** Get linear color map property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static const TMap<FString, FLinearColor>& GetLinearColorMapProperty(UPsData* Target, int32 Crc32);

	/** Set linear color map property */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	static void SetLinearColorMapProperty(UPsData* Target, int32 Crc32, const TMap<FString, FLinearColor>& Value);

	/***********************************
	 * Link
	 ***********************************/

	/** Get path by link hash */
	static const FString& GetLinkPath(const UPsData* Target, TSharedPtr<const FDataLink> Link);

	/** Get keys by link hash */
	static void GetLinkKeys(const UPsData* Target, TSharedPtr<const FDataLink> Link, TArray<FString>& OutKeys);

	/** Get data array property by hash */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static TArray<UPsData*> GetDataArrayByLinkHash(const UPsData* Target, int32 Crc32);

	/** Get data property by hash */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static UPsData* GetDataByLinkHash(const UPsData* Target, int32 Crc32);

	/** Is link empty? */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static bool IsLinkEmpty(const UPsData* Target, int32 Crc32);

	/***********************************
	 * Blueprint collections proxy
	 ***********************************/

	/** Get map proxy property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static UPsDataBlueprintMapProxy* GetMapProxy(UPsData* Target, int32 Crc32);

	/** Get map proxy property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data")
	static UPsDataBlueprintArrayProxy* GetArrayProxy(UPsData* Target, int32 Crc32);
};
