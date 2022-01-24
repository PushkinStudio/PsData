// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "Factories/CSVImportFactory.h"

#include "PsDataReimportDataTableFactory.generated.h"

/***********************************
 * UPsDataDataTableImportFactory
 ***********************************/

UCLASS(hidecategories = Object)
class UPsDataDataTableImportFactory : public UCSVImportFactory
{
	GENERATED_UCLASS_BODY()

protected:
	virtual TArray<FString> DoImportDataTable(const FCSVImportSettings& ImportSettings, class UDataTable* TargetDataTable) override;
};

/***********************************
 * UPsDataDataReimportDataTableFactory
 ***********************************/

UCLASS()
class UPsDataDataReimportDataTableFactory : public UPsDataDataTableImportFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
};
