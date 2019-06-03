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
#if WITH_HOT_RELOAD
	if (!Singleton)
	{
		Init();
	}
	return Singleton;
#else
	check(Singleton);
	return Singleton;
#endif //WITH_HOT_RELOAD
}
