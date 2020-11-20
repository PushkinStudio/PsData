// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsDataDeferredTask.h"

namespace PsDataTools
{

bool FDeferredUniqueTaskHelper::Register(void* Owner, const FGuid& FunctionGuid)
{
	auto& Set = Registered.FindOrAdd(Owner);
	bool bIsAlreadyInSetPtr = false;
	Set.Add(FunctionGuid, &bIsAlreadyInSetPtr);
	return !bIsAlreadyInSetPtr;
}

void FDeferredUniqueTaskHelper::Unregister(void* Owner, const FGuid& FunctionGuid)
{
	auto& Set = Registered.FindChecked(Owner);
	bool bSuccess = Set.Remove(FunctionGuid) == 1;
	check(bSuccess);

	if (Set.Num() == 0)
	{
		Registered.Remove(Owner);
	}
}

TMap<void*, TSet<FGuid>> FDeferredUniqueTaskHelper::Registered;

} // namespace PsDataTools