// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Toolkits/IToolkitHost.h"

class FPsDataAssetTypeActions_DataTable : public FAssetTypeActions_Base
{
public:
	virtual FColor GetTypeColor() const override { return FColor(62, 140, 35); }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return true; }
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Misc; }
	virtual bool IsImportedAsset() const override { return true; }
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_DataTable", "Data Table"); }
	virtual UClass* GetSupportedClass() const override { return UDataTable::StaticClass(); }
	virtual void GetActions(const TArray<UObject*>& InObjects, struct FToolMenuSection& Section) override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
	virtual void PerformAssetDiff(UObject* Asset1, UObject* Asset2, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const override;

protected:
	/** Handler for when CSV is selected */
	void ExecuteExportAsCSV(TArray<TWeakObjectPtr<UObject>> Objects);

	/** Handler for when JSON is selected */
	void ExecuteExportAsJSON(TArray<TWeakObjectPtr<UObject>> Objects);

	/** Handler for opening the source file for this asset */
	void ExecuteFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions);

	/** Determine whether the find source file in explorer editor command can execute or not */
	bool CanExecuteFindSourceFileInExplorer(TArray<FString> Filenames, TArray<FString> OverrideExtensions) const;

	/** Verify the specified filename exists */
	bool VerifyFileExists(const FString& InFileName) const;
};
