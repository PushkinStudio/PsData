// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "Pins/PsDataPinFactory.h"
#include "Pins/PsGraphPinEventList.h"
#include "PsDataNode_Event.h"

#include "Engine/CurveTable.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EdGraphSchema_K2.h"
#include "SGraphPin.h"
	
TSharedPtr<class SGraphPin> FPsDataPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_String && InPin->GetOuter()->IsA<UPsDataNode_Event>())
	{
		TArray<TSharedPtr<FEventPath>> Rows;
		for (auto It = Cast<UPsDataNode_Event>(InPin->GetOuter())->Events.CreateIterator(); It; ++It)
		{
			Rows.Add(MakeShareable(new FEventPath(*It)));
		}
		return SNew(SPsGraphPinEventList, InPin, Rows);
	}

	return nullptr;
}
