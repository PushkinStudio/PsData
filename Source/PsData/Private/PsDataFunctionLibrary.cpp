// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsDataFunctionLibrary.h"

#include "Collection/PsDataBlueprintArrayProxy.h"
#include "Collection/PsDataBlueprintMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"

/***********************************
 * Blueprint collections proxy
 ***********************************/

UPsDataBlueprintMapProxy* UPsDataFunctionLibrary::GetMapProxy(UPsData* Target, int32 Index)
{
	// TODO: Always NewObject?
	UPsDataBlueprintMapProxy* Result = NewObject<UPsDataBlueprintMapProxy>();
	Result->Init(Target, PsDataTools::FDataReflection::GetFieldsByClass(Target->GetClass())->GetFieldByIndex(Index));
	return Result;
}

UPsDataBlueprintArrayProxy* UPsDataFunctionLibrary::GetArrayProxy(UPsData* Target, int32 Index)
{
	// TODO: Always NewObject?
	UPsDataBlueprintArrayProxy* Result = NewObject<UPsDataBlueprintArrayProxy>();
	Result->Init(Target, PsDataTools::FDataReflection::GetFieldsByClass(Target->GetClass())->GetFieldByIndex(Index));
	return Result;
}
