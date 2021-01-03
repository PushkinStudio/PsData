// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#include "Pins/PsDataGraphPinEventList.h"

#include "ScopedTransaction.h"

FString GenerateName(TSharedPtr<FEventPath> Event)
{
	if (Event->bSelf)
	{
		return Event->Type;
	}
	return *FString::Printf(TEXT("%s (%s)"), *Event->Type, *Event->Path);
}

void SPsDataGraphPinEventList::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, const TArray<TSharedPtr<FEventPath>>& InEventList)
{
	EventList = InEventList;

	for (const auto EventPtr : EventList)
	{
		NameList.Add(MakeShareable(new FName(*GenerateName(EventPtr))));
	}

	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget> SPsDataGraphPinEventList::GetDefaultValueWidget()
{
	TSharedPtr<FName> CurrentlySelectedName;

	if (GraphPinObj)
	{
		// Preserve previous selection, or set to first in list
		const FString PreviousSelection = GraphPinObj->GetDefaultAsString();
		for (int32 i = 0; i < EventList.Num(); ++i)
		{
			const auto EventPtr = EventList[i];
			if (PreviousSelection == EventPtr->Type)
			{
				CurrentlySelectedName = NameList[i];
				break;
			}
		}
	}

	// Create widget
	return SAssignNew(ComboBox, SNameComboBox)
		.ContentPadding(FMargin(6.0f, 2.0f))
		.OptionsSource(&NameList)
		.InitiallySelectedItem(CurrentlySelectedName)
		.OnSelectionChanged(this, &SPsDataGraphPinEventList::ComboBoxSelectionChanged)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility);
}

void SPsDataGraphPinEventList::ComboBoxSelectionChanged(TSharedPtr<FName> NameItem, ESelectInfo::Type SelectInfo)
{
	if (const auto Schema = (GraphPinObj ? GraphPinObj->GetSchema() : NULL))
	{
		const FName Name = NameItem.IsValid() ? *NameItem : NAME_None;
		const FString NameAsString = Name.ToString();

		TSharedPtr<FEventPath> Event;

		for (const auto EventPtr : EventList)
		{
			if (NameAsString == GenerateName(EventPtr))
			{
				Event = EventPtr;
				break;
			}
		}

		if (Event.IsValid())
		{
			if (GraphPinObj->GetDefaultAsString() != Event->Type)
			{
				GraphPinObj->Modify();
				Schema->TrySetDefaultValue(*GraphPinObj, Event->Type);
			}
		}
	}
}
