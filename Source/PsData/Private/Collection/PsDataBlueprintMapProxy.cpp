// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Collection/PsDataBlueprintMapProxy.h"

#include "PsDataCore.h"

UPsDataBlueprintMapProxy::UPsDataBlueprintMapProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsDataBlueprintMapProxy::Init(PsDataTools::TDataProperty<TMap<FString, UPsData*>>* InProperty)
{
	Property = InProperty;
}

bool UPsDataBlueprintMapProxy::IsValid()
{
	return true;
}

void UPsDataBlueprintMapProxy::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	TPsDataMapProxy<UPsData*>(Property).Bind(Type, Delegate);
}

void UPsDataBlueprintMapProxy::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	TPsDataMapProxy<UPsData*>(Property).Unbind(Type, Delegate);
}

TMap<FString, UPsData*> UPsDataBlueprintMapProxy::Get()
{
	return Property->GetValue();
}
