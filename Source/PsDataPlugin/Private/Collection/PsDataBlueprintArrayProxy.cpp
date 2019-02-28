// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Collection/PsDataBlueprintArrayProxy.h"

#include "PsDataCore.h"

UPsDataBlueprintArrayProxy::UPsDataBlueprintArrayProxy(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Proxy()
{
}

void UPsDataBlueprintArrayProxy::Init(UPsData* Instance, const TSharedPtr<const FDataField> Field)
{
	check(!Proxy.IsValid());
	Proxy = FPsDataArrayProxy<UPsData*>(Instance, Field);
	check(IsValid());
}

bool UPsDataBlueprintArrayProxy::IsValid()
{
	return Proxy.IsValid();
}

void UPsDataBlueprintArrayProxy::Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	Proxy.Bind(Type, Delegate);
}

void UPsDataBlueprintArrayProxy::Bind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	Proxy.Bind(Type, Delegate);
}

void UPsDataBlueprintArrayProxy::Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	Proxy.Unbind(Type, Delegate);
}

void UPsDataBlueprintArrayProxy::Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	Proxy.Unbind(Type, Delegate);
}

FPsDataConstArrayProxy<UPsData*> UPsDataBlueprintArrayProxy::GetProxy()
{
	return Proxy;
}

void UPsDataBlueprintArrayProxy::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	Proxy.Bind(Type, Delegate);
}

void UPsDataBlueprintArrayProxy::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	Proxy.Unbind(Type, Delegate);
}

const TArray<UPsData*>& UPsDataBlueprintArrayProxy::GetRef()
{
	return Proxy.GetRef();
}
