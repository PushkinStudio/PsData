// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Factories/PsDataReimportDataTableFactory.h"

#include "Serialize/PsDataStructSerialization.h"
#include "Serialize/PsDataTableSerialization.h"

#include "EditorFramework/AssetImportData.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"

/***********************************
 * UPsDataDataTableImportFactory
 ***********************************/

UPsDataDataTableImportFactory::UPsDataDataTableImportFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ImportAsJson(TArray<FString>& ImportProblems, const FString& DataToImport, UDataTable* DataTable)
{
	if (DataToImport.IsEmpty())
	{
		ImportProblems.Add(TEXT("Input data is empty."));
		return;
	}

	if (!DataTable->RowStruct)
	{
		ImportProblems.Add(TEXT("No RowStruct specified."));
		return;
	}

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(DataToImport);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonArray) || JsonArray.Num() == 0)
	{
		ImportProblems.Add(FString::Printf(TEXT("Failed to parse the JSON data. Error: %s"), *JsonReader->GetErrorMessage()));
		return;
	}

	FPsDataTableSerializer::CreateTableFromJson_Import(DataTable, JsonArray, ImportProblems);
	DataTable->Modify(true);
}

#if OLD_CSV_IMPORT_FACTORY
TArray<FString> UPsDataDataTableImportFactory::DoImportDataTable(class UDataTable* TargetDataTable, const FString& DataToImport)
{
	const bool bIsJSON = CurrentFilename.EndsWith(TEXT(".json"));
	if (bIsJSON)
	{
		TArray<FString> ImportProblems;
		ImportAsJson(ImportProblems, DataToImport, TargetDataTable);
		return ImportProblems;
	}

	return TargetDataTable->CreateTableFromCSVString(DataToImport);
}
#else
TArray<FString> UPsDataDataTableImportFactory::DoImportDataTable(const FCSVImportSettings& ImportSettings, class UDataTable* TargetDataTable)
{
	const bool bIsJSON = CurrentFilename.EndsWith(TEXT(".json"));
	if (bIsJSON)
	{
		TArray<FString> ImportProblems;
		ImportAsJson(ImportProblems, ImportSettings.DataToImport, TargetDataTable);
		return ImportProblems;
	}

	return TargetDataTable->CreateTableFromCSVString(ImportSettings.DataToImport);
}
#endif

/***********************************
 * UPsDataDataReimportDataTableFactory
 ***********************************/

UPsDataDataReimportDataTableFactory::UPsDataDataReimportDataTableFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("json;JavaScript Object Notation"));
}

bool UPsDataDataReimportDataTableFactory::FactoryCanImport(const FString& Filename)
{
	return true;
}

bool UPsDataDataReimportDataTableFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UDataTable* DataTable = Cast<UDataTable>(Obj);
	if (DataTable)
	{
		DataTable->AssetImportData->ExtractFilenames(OutFilenames);

		return true;
	}
	return false;
}

void UPsDataDataReimportDataTableFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UDataTable* DataTable = Cast<UDataTable>(Obj);
	if (DataTable && ensure(NewReimportPaths.Num() == 1))
	{
		DataTable->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UPsDataDataReimportDataTableFactory::Reimport(UObject* Obj)
{
	EReimportResult::Type Result = EReimportResult::Failed;
	if (UDataTable* DataTable = Cast<UDataTable>(Obj))
	{
		Result = UPsDataDataTableImportFactory::ReimportCSV(DataTable) ? EReimportResult::Succeeded : EReimportResult::Failed;
	}
	return Result;
}

int32 UPsDataDataReimportDataTableFactory::GetPriority() const
{
	return UFactory::GetDefaultImportPriority() + 1;
}