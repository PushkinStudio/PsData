// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

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
#if !UE_BUILD_SHIPPING
	const auto Field = PsDataTools::FDataReflection::GetFieldsByClass(Target->GetClass())->GetFieldByIndex(Index);
	TMap<FString, UPsData*>* MapPtr = nullptr;
	PsDataTools::GetByField<true>(Target, Field, MapPtr);
#endif

	const auto Property = PsDataTools::FPsDataFriend::GetProperty(Target, Index);

	// TODO: Always NewObject?
	UPsDataBlueprintMapProxy* Result = NewObject<UPsDataBlueprintMapProxy>();
	Result->Init(static_cast<PsDataTools::TDataProperty<TMap<FString, UPsData*>>*>(Property));
	return Result;
}

UPsDataBlueprintArrayProxy* UPsDataFunctionLibrary::GetArrayProxy(UPsData* Target, int32 Index)
{
#if !UE_BUILD_SHIPPING
	const auto Field = PsDataTools::FDataReflection::GetFieldsByClass(Target->GetClass())->GetFieldByIndex(Index);
	TArray<UPsData*>* ArrayPtr = nullptr;
	PsDataTools::GetByField<true>(Target, Field, ArrayPtr);
#endif

	const auto Property = PsDataTools::FPsDataFriend::GetProperty(Target, Index);

	// TODO: Always NewObject?
	UPsDataBlueprintArrayProxy* Result = NewObject<UPsDataBlueprintArrayProxy>();
	Result->Init(static_cast<PsDataTools::TDataProperty<TArray<UPsData*>>*>(Property));
	return Result;
}
