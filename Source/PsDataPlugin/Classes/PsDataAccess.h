// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PsDataAccess.generated.h"

UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class PSDATAPLUGIN_API UPsDataAccess : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class PSDATAPLUGIN_API IPsDataAccess
{
	GENERATED_IINTERFACE_BODY()
	
	virtual bool CanModify() { return true; };
	virtual bool DeferredEventProcessing() { return false; };
};
