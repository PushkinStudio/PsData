// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataEditorModule.h"

#include "AssetTypeActions/PsDataAssetTypeActions_DataTable.h"
#include "Customization/PsDataBigIntegerStructCustomization.h"
#include "Customization/PsDataFixedPointStructCustomization.h"
#include "Pins/PsDataPinFactory.h"
#include "PsData.h"
#include "PsDataDefines.h"

#include "AssetToolsModule.h"
#include "EdGraphUtilities.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "PsDataEditorModule"

void FPsDataEditorModule::StartupModule()
{
	const TSharedPtr<FPsDataPinFactory> PinFactory = MakeShareable(new FPsDataPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("PsDataFixedPoint", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPsDataFixedPointStructCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("PsDataBigInteger", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FPsDataBigIntegerStructCustomization::MakeInstance));

#if PSDATA_ENGINE_AT_LEAST(5, 8)
	FCoreDelegates::GetOnPostEngineInit().AddLambda([this]() {
#else
	FCoreDelegates::OnPostEngineInit.AddLambda([this]() {
#endif
		RegisterAssetTypeActions();
	});
}

void FPsDataEditorModule::ShutdownModule()
{
	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		PropertyModule->UnregisterCustomPropertyTypeLayout("PsDataFixedPoint");
		PropertyModule->UnregisterCustomPropertyTypeLayout("PsDataBigInteger");
	}
}

void FPsDataEditorModule::RegisterAssetTypeActions()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataEditorModule, PsDataEditor)
