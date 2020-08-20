// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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