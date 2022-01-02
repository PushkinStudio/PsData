// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataEditorModule.h"

#include "Pins/PsDataPinFactory.h"
#include "PsData.h"

#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "PsDataEditorModule"

void FPsDataEditorModule::StartupModule()
{
	const TSharedPtr<FPsDataPinFactory> PinFactory = MakeShareable(new FPsDataPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
}

void FPsDataEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataEditorModule, PsDataEditor)
