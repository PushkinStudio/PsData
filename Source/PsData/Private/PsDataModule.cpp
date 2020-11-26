// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "PsDataModule.h"

#include "PsDataCore.h"

#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FPsDataModule"

void FPsDataModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([] {
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
