// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataHardObjectPtr.h"

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
		Singleton->SetFlags(RF_Standalone);
	}

	return Singleton;
}
