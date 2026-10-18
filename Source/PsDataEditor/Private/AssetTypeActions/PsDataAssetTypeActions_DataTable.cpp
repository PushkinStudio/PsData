// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "AssetTypeActions/PsDataAssetTypeActions_DataTable.h"

#include "PsDataDefines.h"
#include "Serialize/PsDataTableSerialization.h"

#include "AssetToolsModule.h"
#include "ContentBrowserMenuContexts.h"
#include "DesktopPlatformModule.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Editor/DataTableEditor/Public/DataTableEditorModule.h"
#include "EditorFramework/AssetImportData.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

namespace PsData_MenuExtension_DataTable
{
void ExecuteExportAsCSV(const FToolMenuContext& InContext)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(InContext);
	for (UDataTable* DataTable : Context->LoadSelectedObjects<UDataTable>())
	{
		const FText Title = FText::Format(LOCTEXT("DataTable_ExportCSVDialogTitle", "Export '{0}' as CSV..."), FText::FromString(*DataTable->GetName()));
		const FString CurrentFilename = DataTable->AssetImportData->GetFirstFilename();
		const FString FileTypes = TEXT("Data Table CSV (*.csv)|*.csv");

		TArray<FString> OutFilenames;
		DesktopPlatform->SaveFileDialog(
			ParentWindowWindowHandle,
			Title.ToString(),
			(CurrentFilename.IsEmpty()) ? TEXT("") : FPaths::GetPath(CurrentFilename),
			(CurrentFilename.IsEmpty()) ? TEXT("") : FPaths::GetBaseFilename(CurrentFilename) + TEXT(".csv"),
			FileTypes,
			EFileDialogFlags::None,
			OutFilenames);

		if (OutFilenames.Num() > 0)
		{
			FFileHelper::SaveStringToFile(DataTable->GetTableAsCSV(), *OutFilenames[0]);
		}
	}
}

void ExecuteExportAsJSON(const FToolMenuContext& InContext)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	const UContentBrowserAssetContextMenuContext* Context = UContentBrowserAssetContextMenuContext::FindContextWithAssets(InContext);
	for (UDataTable* DataTable : Context->LoadSelectedObjects<UDataTable>())
	{
		const FText Title = FText::Format(LOCTEXT("DataTable_ExportJSONDialogTitle", "Export '{0}' as JSON..."), FText::FromString(*DataTable->GetName()));
		const FString CurrentFilename = DataTable->AssetImportData->GetFirstFilename();
		const FString FileTypes = TEXT("Data Table JSON (*.json)|*.json");

		TArray<FString> OutFilenames;
		DesktopPlatform->SaveFileDialog(
			ParentWindowWindowHandle,
			Title.ToString(),
			(CurrentFilename.IsEmpty()) ? TEXT("") : FPaths::GetPath(CurrentFilename),
			(CurrentFilename.IsEmpty()) ? TEXT("") : FPaths::GetBaseFilename(CurrentFilename) + TEXT(".json"),
			FileTypes,
			EFileDialogFlags::None,
			OutFilenames);

		if (OutFilenames.Num() > 0)
		{
			auto JsonArray = FPsDataTableDeserializer::CreateJsonFromTable_Export(DataTable);
			FString JsonString;
			auto Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonString);
			FJsonSerializer::Serialize(JsonArray, Writer);
			FFileHelper::SaveStringToFile(JsonString, *OutFilenames[0]);
		}
	}
}

static FDelayedAutoRegisterHelper DelayedAutoRegister(EDelayedRegisterRunPhase::EndOfEngineInit, [] {
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda([]() {
		FToolMenuOwnerScoped OwnerScoped(UE_MODULE_NAME);
		UToolMenu* Menu = UE::ContentBrowser::ExtendToolMenu_AssetContextMenu(UDataTable::StaticClass());

		FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
		Section.AddDynamicEntry(NAME_None, FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection) {
			{
				const TAttribute<FText> Label = LOCTEXT("DataTable_ExportAsCSV_PsData", "Export as CSV (PsData)");
				const TAttribute<FText> ToolTip = LOCTEXT("DataTable_ExportAsCSVTooltip_PsData", "Export the data table as a file containing CSV data.");
				const FSlateIcon Icon = FSlateIcon();

				FToolUIAction UIAction;
				UIAction.ExecuteAction = FToolMenuExecuteAction::CreateStatic(&ExecuteExportAsCSV);
				InSection.AddMenuEntry("DataTable_ExportAsCSV_PsData", Label, ToolTip, Icon, UIAction);
			}

			{
				const TAttribute<FText> Label = LOCTEXT("DataTable_ExportAsJSON_PsData", "Export as JSON (PsData)");
				const TAttribute<FText> ToolTip = LOCTEXT("DataTable_ExportAsJSONTooltip_PsData", "Export the data table as a file containing JSON data.");
				const FSlateIcon Icon = FSlateIcon();

				FToolUIAction UIAction;
				UIAction.ExecuteAction = FToolMenuExecuteAction::CreateStatic(&ExecuteExportAsJSON);
				InSection.AddMenuEntry("DataTable_ExportAsJSON_PsData", Label, ToolTip, Icon, UIAction);
			}
		}));
	}));
});
} // namespace PsData_MenuExtension_DataTable

#undef LOCTEXT_NAMESPACE