// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataHardObjectPtr.h"

/***********************************
 * FHardObjectPtrCounter
 ***********************************/

FHardObjectPtrCounter::FHardObjectPtrCounter()
	: Num(0)
	, Object(nullptr)
{
}

FHardObjectPtrCounter::FHardObjectPtrCounter(const UObject* InObject)
	: Num(0)
	, Object(InObject)
{
	check(Object);
}

/***********************************
* UPsDataSingleton
***********************************/

UPsDataHardObjectPtrSingleton* UPsDataHardObjectPtrSingleton::Singleton = nullptr;

UPsDataHardObjectPtrSingleton::UPsDataHardObjectPtrSingleton(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	check(!Singleton);
}

UPsDataHardObjectPtrSingleton* UPsDataHardObjectPtrSingleton::Get()
{
	if (Singleton == nullptr)
	{
		Singleton = NewObject<UPsDataHardObjectPtrSingleton>();
		Singleton->AddToRoot();
	}

	return Singleton;
}
