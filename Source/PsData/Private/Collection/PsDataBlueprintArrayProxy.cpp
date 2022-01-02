// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Collection/PsDataBlueprintArrayProxy.h"

#include "PsDataCore.h"

UPsDataBlueprintArrayProxy::UPsDataBlueprintArrayProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsDataBlueprintArrayProxy::Init(PsDataTools::TDataProperty<TArray<UPsData*>>* InProperty)
{
	Property = InProperty;
}

bool UPsDataBlueprintArrayProxy::IsValid()
{
	return true;
}

void UPsDataBlueprintArrayProxy::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	TPsDataArrayProxy<UPsData*>(Property).Bind(Type, Delegate);
}

void UPsDataBlueprintArrayProxy::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	TPsDataArrayProxy<UPsData*>(Property).Unbind(Type, Delegate);
}

TArray<UPsData*> UPsDataBlueprintArrayProxy::Get()
{
	return Property->GetValue();
}
