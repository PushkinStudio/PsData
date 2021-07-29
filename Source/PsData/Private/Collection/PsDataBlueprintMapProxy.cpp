// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Collection/PsDataBlueprintMapProxy.h"

#include "PsDataCore.h"

UPsDataBlueprintMapProxy::UPsDataBlueprintMapProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsDataBlueprintMapProxy::Init(UPsData* Instance, const TSharedPtr<const FDataField> Field)
{
	check(!Proxy.IsValid());
	Proxy = FPsDataMapProxy<UPsData*>(Instance, Field);
	check(IsValid());
}

bool UPsDataBlueprintMapProxy::IsValid()
{
	return Proxy.IsValid();
}

void UPsDataBlueprintMapProxy::Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	Proxy.Bind(Type, Delegate);
}

void UPsDataBlueprintMapProxy::Bind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	Proxy.Bind(Type, Delegate);
}

void UPsDataBlueprintMapProxy::Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	Proxy.Unbind(Type, Delegate);
}

void UPsDataBlueprintMapProxy::Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	Proxy.Unbind(Type, Delegate);
}

FPsDataConstMapProxy<UPsData*> UPsDataBlueprintMapProxy::GetProxy()
{
	return Proxy;
}

void UPsDataBlueprintMapProxy::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	Proxy.Bind(Type, Delegate);
}

void UPsDataBlueprintMapProxy::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	Proxy.Unbind(Type, Delegate);
}

TMap<FString, UPsData*> UPsDataBlueprintMapProxy::Get()
{
	return Proxy.Get();
}
