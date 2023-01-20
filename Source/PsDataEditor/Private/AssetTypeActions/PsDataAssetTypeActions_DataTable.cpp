// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "AssetTypeActions/PsDataAssetTypeActions_DataTable.h"

#include "PsDataDefines.h"
#include "Serialize/PsDataTableSerialization.h"

#include "AssetToolsModule.h"
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

void FPsDataAssetTypeActions_DataTable::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	auto Tables = GetTypedWeakObjectPtrs<UObject>(InObjects);

	TArray<FString> ImportPaths;
	for (auto TableIter = Tables.CreateConstIterator(); TableIter; ++TableIter)
	{
		const UDataTable* CurTable = Cast<UDataTable>((*TableIter).Get());
		if (CurTable)
		{
			CurTable->AssetImportData->ExtractFilenames(ImportPaths);
		}
	}

	Section.AddMenuEntry(
		"DataTable_ExportAsCSV",
		LOCTEXT("DataTable_ExportAsCSV", "Export as CSV"),
		LOCTEXT("DataTable_ExportAsCSVTooltip", "Export the data table as a file containing CSV data."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPsDataAssetTypeActions_DataTable::ExecuteExportAsCSV, Tables),
			FCanExecuteAction()));

	Section.AddMenuEntry(
		"DataTable_ExportAsJSON",
		LOCTEXT("DataTable_ExportAsJSON", "Export as JSON"),
		LOCTEXT("DataTable_ExportAsJSONTooltip", "Export the data table as a file containing JSON data."),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPsDataAssetTypeActions_DataTable::ExecuteExportAsJSON, Tables),
			FCanExecuteAction()));

	TArray<FString> PotentialFileExtensions;
	PotentialFileExtensions.Add(TEXT(".xls"));
	PotentialFileExtensions.Add(TEXT(".xlsm"));
	PotentialFileExtensions.Add(TEXT(".csv"));
	PotentialFileExtensions.Add(TEXT(".json"));
	Section.AddMenuEntry(
		"DataTable_OpenSourceData",
		LOCTEXT("DataTable_OpenSourceData", "Open Source Data"),
		LOCTEXT("DataTable_OpenSourceDataTooltip", "Opens the data table's source data file in an external editor. It will search using the following extensions: .xls/.xlsm/.csv/.json"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FPsDataAssetTypeActions_DataTable::ExecuteFindSourceFileInExplorer, ImportPaths, PotentialFileExtensions),
			FCanExecuteAction::CreateSP(this, &FPsDataAssetTypeActions_DataTable::CanExecuteFindSourceFileInExplorer, ImportPaths, PotentialFileExtensions)));
}

void FPsDataAssetTypeActions_DataTable::ExecuteExportAsCSV(TArray<TWeakObjectPtr<UObject>> Objects)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto DataTable = Cast<UDataTable>((*ObjIt).Get());
		if (DataTable)
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
}

void FPsDataAssetTypeActions_DataTable::ExecuteExportAsJSON(TArray<TWeakObjectPtr<UObject>> Objects)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto DataTable = Cast<UDataTable>((*ObjIt).Get());
		if (DataTable)
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
}

void FPsDataAssetTypeActions_DataTable::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	TArray<UDataTable*> DataTablesToOpen;
	TArray<UDataTable*> InvalidDataTables;

	for (UObject* Obj : InObjects)
	{
		UDataTable* Table = Cast<UDataTable>(Obj);
		if (Table)
		{
			if (Table->GetRowStruct())
			{
				DataTablesToOpen.Add(Table);
			}
			else
			{
				InvalidDataTables.Add(Table);
			}
		}
	}

	if (InvalidDataTables.Num() > 0)
	{
		FTextBuilder DataTablesListText;
		DataTablesListText.Indent();
		for (UDataTable* Table : InvalidDataTables)
		{
			const FName ResolvedRowStructName = Table->GetRowStructName();
			DataTablesListText.AppendLineFormat(LOCTEXT("DataTable_MissingRowStructListEntry", "* {0} (Row Structure: {1})"), FText::FromString(Table->GetName()), FText::FromName(ResolvedRowStructName));
		}

		FText Title = LOCTEXT("DataTable_MissingRowStructTitle", "Continue?");
		const EAppReturnType::Type DlgResult = FMessageDialog::Open(
			EAppMsgType::YesNoCancel,
			FText::Format(LOCTEXT("DataTable_MissingRowStructMsg", "The following Data Tables are missing their row structure and will not be editable.\n\n{0}\n\nDo you want to open these data tables?"), DataTablesListText.ToText()),
			&Title);

		switch (DlgResult)
		{
		case EAppReturnType::Yes:
			DataTablesToOpen.Append(InvalidDataTables);
			break;
		case EAppReturnType::Cancel:
			return;
		default:
			break;
		}
	}

	FDataTableEditorModule& DataTableEditorModule = FModuleManager::LoadModuleChecked<FDataTableEditorModule>("DataTableEditor");
	for (UDataTable* Table : DataTablesToOpen)
	{
		DataTableEditorModule.CreateDataTableEditor(EToolkitMode::Standalone, EditWithinLevelEditor, Table);
	}
}

void FPsDataAssetTypeActions_DataTable::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (auto& Asset : TypeAssets)
	{
		const auto DataTable = CastChecked<UDataTable>(Asset);
		DataTable->AssetImportData->ExtractFilenames(OutSourceFilePaths);
	}
}

void FPsDataAssetTypeActions_DataTable::PerformAssetDiff(UObject* OldAsset, UObject* NewAsset, const FRevisionInfo& OldRevision, const FRevisionInfo& NewRevision) const
{
	UDataTable* OldDataTable = CastChecked<UDataTable>(OldAsset);
	UDataTable* NewDataTable = CastChecked<UDataTable>(NewAsset);

	FString RelOldTempFileName = FString::Printf(TEXT("%sTemp%s-%s.csv"), *FPaths::DiffDir(), *OldAsset->GetName(), *OldRevision.Revision);
	FString AbsoluteOldTempFileName = FPaths::ConvertRelativePathToFull(RelOldTempFileName);
	FString RelNewTempFileName = FString::Printf(TEXT("%sTemp%s-%s.csv"), *FPaths::DiffDir(), *NewAsset->GetName(), *NewRevision.Revision);
	FString AbsoluteNewTempFileName = FPaths::ConvertRelativePathToFull(RelNewTempFileName);

#if OLD_CSV_IMPORT_FACTORY
	auto const DefaultExportFlags = EDataTableExportFlags::None;
#else
	auto const DefaultExportFlags = EDataTableExportFlags::UseSimpleText;
#endif

	bool OldResult = FFileHelper::SaveStringToFile(OldDataTable->GetTableAsCSV(DefaultExportFlags), *AbsoluteOldTempFileName);
	bool NewResult = FFileHelper::SaveStringToFile(NewDataTable->GetTableAsCSV(DefaultExportFlags), *AbsoluteNewTempFileName);

	if (OldResult && NewResult)
	{
		FString DiffCommand = GetDefault<UEditorLoadingSavingSettings>()->TextDiffToolPath.FilePath;

		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateDiffProcess(DiffCommand, AbsoluteOldTempFileName, AbsoluteNewTempFileName);
	}
	else
	{
		FPsDataAssetTypeActions_DataTable::PerformAssetDiff(OldAsset, NewAsset, OldRevision, NewRevision);
	}
}

void FPsDataAssetTypeActions_DataTable::ExecuteFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions)
{
	for (TArray<FString>::TConstIterator FilenameIter(Filenames); FilenameIter; ++FilenameIter)
	{
		const FString CSVFilename = FPaths::ConvertRelativePathToFull(*FilenameIter);
		const FString RootPath = FPaths::GetPath(CSVFilename);
		const FString BaseFilename = FPaths::GetBaseFilename(CSVFilename, true);

		for (TArray<FString>::TConstIterator ExtensionItr(OverrideExtensions); ExtensionItr; ++ExtensionItr)
		{
			const FString FilenameWithExtension(FString::Printf(TEXT("%s/%s%s"), *RootPath, *BaseFilename, **ExtensionItr));
			if (VerifyFileExists(FilenameWithExtension))
			{
				FPlatformProcess::LaunchFileInDefaultExternalApplication(*FilenameWithExtension, NULL, ELaunchVerb::Edit);
				break;
			}
		}
	}
}

bool FPsDataAssetTypeActions_DataTable::CanExecuteFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions) const
{
	if (OverrideExtensions.Num() == 0)
	{
		return false;
	}

	for (TArray<FString>::TConstIterator FilenameIter(Filenames); FilenameIter; ++FilenameIter)
	{
		const FString CSVFilename = FPaths::ConvertRelativePathToFull(*FilenameIter);
		const FString RootPath = FPaths::GetPath(CSVFilename);
		const FString BaseFilename = FPaths::GetBaseFilename(CSVFilename, true);

		for (TArray<FString>::TConstIterator ExtensionItr(OverrideExtensions); ExtensionItr; ++ExtensionItr)
		{
			const FString FilenameWithExtension(FString::Printf(TEXT("%s/%s%s"), *RootPath, *BaseFilename, **ExtensionItr));
			if (VerifyFileExists(FilenameWithExtension))
			{
				return true;
			}
		}
	}

	return false;
}

bool FPsDataAssetTypeActions_DataTable::VerifyFileExists(const FString& InFileName) const
{
	return (!InFileName.IsEmpty() && IFileManager::Get().FileSize(*InFileName) != INDEX_NONE);
}

#undef LOCTEXT_NAMESPACE
