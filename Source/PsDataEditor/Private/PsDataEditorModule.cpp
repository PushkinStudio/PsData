// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "PsDataEditorModule.h"

#include "AssetTypeActions/PsDataAssetTypeActions_DataTable.h"
#include "Customization/PsDataBigIntegerStructCustomization.h"
#include "Customization/PsDataFixedPointStructCustomization.h"
#include "Pins/PsDataPinFactory.h"
#include "PsData.h"

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

	FCoreDelegates::OnPostEngineInit.AddLambda([this]() {
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
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	const auto AssetTypeActionsForDataTable = AssetTools.GetAssetTypeActionsForClass(UDataTable::StaticClass());
	if (AssetTypeActionsForDataTable.IsValid())
	{
		AssetTools.UnregisterAssetTypeActions(AssetTypeActionsForDataTable.Pin().ToSharedRef());
	}

	AssetTools.RegisterAssetTypeActions(MakeShared<FPsDataAssetTypeActions_DataTable>());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPsDataEditorModule, PsDataEditor)
