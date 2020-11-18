// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsDataPlugin.h"

#include "PsDataCore.h"
#include "PsDataHardObjectPtr.h"

#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FPsDataPluginModule"

void FPsDataPluginModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([] {
		PsDataTools::FDataReflection::Compile();
		UPsDataHardObjectPtrSingleton::Init();

		FDataDelegates::OnPostDataModuleInit.Broadcast();
		FDataDelegates::PostDataModuleInitPromise.Resolve();
	});
}

void FPsDataPluginModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataPluginModule, PsDataPlugin)
