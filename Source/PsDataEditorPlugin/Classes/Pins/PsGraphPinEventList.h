// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"
#include "SNameComboBox.h"
#include "PsData.h"
#include "PsDataNode_Event.h"

class PSDATAEDITORPLUGIN_API SPsGraphPinEventList : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SPsGraphPinEventList) {}
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, const TArray<TSharedPtr<FEventPath>>& InEventList);
	
protected:
	
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	
	void ComboBoxSelectionChanged(TSharedPtr<FName> NameItem, ESelectInfo::Type SelectInfo);
	
	TSharedPtr<class SNameComboBox>	ComboBox;
	
	TArray<TSharedPtr<FEventPath>> EventList;
	TArray<TSharedPtr<FName>> NameList;
};
