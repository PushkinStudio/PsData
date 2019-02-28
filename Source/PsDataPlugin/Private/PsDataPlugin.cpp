// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataPlugin.h"

#include "PsDataCore.h"

#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FPsDataPluginModule"

void FPsDataPluginModule::StartupModule()
{
	FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([] {
		FDataReflection::Compile();
	});
}

void FPsDataPluginModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataPluginModule, PsDataPlugin)
