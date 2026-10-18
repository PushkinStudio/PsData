// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataModule.h"

#include "PsDataCore.h"
#include "PsDataDefines.h"

#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FPsDataModule"

void FPsDataModule::StartupModule()
{
#if PSDATA_ENGINE_AT_LEAST(5, 8)
	FCoreDelegates::GetOnPostEngineInit().AddLambda([] {
#else
	FCoreDelegates::OnPostEngineInit.AddLambda([] {
#endif
		PsDataTools::FDataReflection::Compile();

		FDataDelegates::OnPostDataModuleInit.Broadcast();
		FDataDelegates::PostDataModuleInitPromise.Resolve();
	});
}

void FPsDataModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataModule, PsData)
