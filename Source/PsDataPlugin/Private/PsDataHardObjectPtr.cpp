// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

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

void UPsDataHardObjectPtrSingleton::RetainObject(const UObject* Object)
{
	if (Object != nullptr)
	{
		check(!Object->IsPendingKill() && !Object->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed));
		auto& ObjectCounters = Get()->ObjectCounters;
		auto& Count = ObjectCounters.FindOrAdd(Object, 0);

		check(Count >= 0);
		++Count;
	}
}

void UPsDataHardObjectPtrSingleton::ReleaseObject(const UObject* Object)
{
	if (GExitPurge)
	{
		return;
	}

	if (Object != nullptr)
	{
		auto& ObjectCounters = Get()->ObjectCounters;

		if (Object->IsPendingKill() || Object->HasAnyFlags(RF_BeginDestroyed | RF_FinishDestroyed))
		{
			for (auto It = ObjectCounters.CreateIterator(); It; ++It)
			{
				if (It->Key == nullptr)
				{
					It.RemoveCurrent();
				}
			}
		}
		else
		{
			auto& Count = ObjectCounters.FindChecked(Object);

			check(Count > 0);
			--Count;

			if (Count == 0)
			{
				ObjectCounters.Remove(Object);
			}
		}
	}
}
