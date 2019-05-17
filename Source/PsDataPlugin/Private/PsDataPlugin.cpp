// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataPlugin.h"

#include "PsDataCore.h"
#include "PsDataHardObjectPtr.h"

#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FPsDataPluginModule"

void FPsDataPluginModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([] {
		FDataReflection::Compile();
		UPsDataHardObjectPtrSingleton::Init();
	});
}

void FPsDataPluginModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataPluginModule, PsDataPlugin)
