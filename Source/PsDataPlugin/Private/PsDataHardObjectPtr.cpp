// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataHardObjectPtr.h"

/***********************************
* UPsDataSingleton
***********************************/

UPsDataHardObjectPtrSingleton* UPsDataHardObjectPtrSingleton::Singleton = nullptr;

UPsDataHardObjectPtrSingleton::UPsDataHardObjectPtrSingleton(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsDataHardObjectPtrSingleton::Init()
{
	check(!Singleton);
	Singleton = NewObject<UPsDataHardObjectPtrSingleton>();
	Singleton->AddToRoot();
}

UPsDataHardObjectPtrSingleton* UPsDataHardObjectPtrSingleton::Get()
{
	check(Singleton);
	return Singleton;
}
