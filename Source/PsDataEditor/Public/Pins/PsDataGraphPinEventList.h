// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "SNameComboBox.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"

struct FEventPath
{
	/** Event type */
	FString Type;

	/** Event path */
	FString Path;

	/** Self */
	bool bSelf;

	FEventPath(FString InType, FString InPath, bool bInSelf)
		: Type(InType)
		, Path(InPath)
		, bSelf(bInSelf)
	{
	}
};

class PSDATAEDITOR_API SPsDataGraphPinEventList : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SPsDataGraphPinEventList) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, const TArray<TSharedPtr<FEventPath>>& InEventList);

protected:
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	void ComboBoxSelectionChanged(TSharedPtr<FName> NameItem, ESelectInfo::Type SelectInfo);

	TSharedPtr<class SNameComboBox> ComboBox;

	TArray<TSharedPtr<FEventPath>> EventList;
	TArray<TSharedPtr<FName>> NameList;
};
