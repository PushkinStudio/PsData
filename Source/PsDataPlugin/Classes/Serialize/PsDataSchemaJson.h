// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FJsonObject;

//////////////////////////////////////////////////////////////////////////
// FPsDataSchemaJson generates json description for all PsDatas

struct PSDATAPLUGIN_API FPsDataSchemaJson
{
	static TSharedPtr<FJsonObject> Get();
};
