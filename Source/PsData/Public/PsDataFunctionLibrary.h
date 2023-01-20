// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsDataFunctionLibrary.generated.h"

class UPsData;
class UPsDataBlueprintMapProxy;
class UPsDataBlueprintArrayProxy;

UCLASS()
class PSDATA_API UPsDataFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/***********************************
	 * Blueprint collections proxy
	 ***********************************/

	/** Get map proxy property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index"))
	static UPsDataBlueprintMapProxy* GetMapProxy(UPsData* Target, int32 Index);

	/** Get map proxy property */
	UFUNCTION(BlueprintPure, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index"))
	static UPsDataBlueprintArrayProxy* GetArrayProxy(UPsData* Target, int32 Index);
};
