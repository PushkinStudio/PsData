// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "Pins/PsDataPinFactory.h"
#include "Pins/PsGraphPinEventList.h"
#include "Engine/CurveTable.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EdGraphSchema_K2.h"
#include "SGraphPin.h"
#include "K2Node_CallFunction.h"
#include "PsDataAPI.h"
	
TArray<TSharedPtr<FEventPath>> GenerateEvents(UClass* TargetClass)
{
	struct FHelperStruct
	{
	public:
		UClass * Class;
		FString Path;

		FHelperStruct(UClass* InClass, const FString& InPath)
			: Class(InClass)
			, Path(InPath)
		{}
	};

	TArray<FHelperStruct> Children;
	TArray<TSharedPtr<FEventPath>> Events;

	Events.Add(MakeShareable(new FEventPath(TEXT("Added"), TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(TEXT("Removing"), TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(TEXT("Changed"), TEXT(""), true)));
	
	for (auto& Pair : FDataReflection::GetFields(TargetClass))
	{
		if (Pair.Value.Meta.bEvent)
		{
			Events.Add(MakeShareable(new FEventPath(FDataReflection::GenerateChangePropertyEventTypeName(Pair.Value), TEXT(""), true)));
		}

		if (Pair.Value.Type == EDataFieldType::DFT_Data)
		{
			Children.Add(FHelperStruct(Pair.Value.Class, Pair.Value.Name));
		}
	}

	while (Children.Num() > 0)
	{
		FHelperStruct Child = Children.Last();
		Children.RemoveAt(Children.Num() - 1, 1, false);

		for (auto& Pair : FDataReflection::GetFields(Child.Class))
		{
			FString Path = Child.Path + TEXT(".") + Pair.Value.Name;
			if (Pair.Value.Meta.bEvent && Pair.Value.Meta.bBubbles)
			{
				Events.Add(MakeShareable(new FEventPath(FDataReflection::GenerateChangePropertyEventTypeName(Pair.Value), Path, false)));
			}

			if (Pair.Value.Type == EDataFieldType::DFT_Data)
			{
				Children.Add(FHelperStruct(Pair.Value.Class, Path));
			}
		}
	}

	return Events;
}

TSharedPtr<class SGraphPin> FPsDataPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	UFunction* BindFunction = UPsData::StaticClass()->FindFunctionByName("BlueprintBind");
	UFunction* UnbindFunction = UPsData::StaticClass()->FindFunctionByName("BlueprintUnbind");

	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_String && InPin->GetOuter()->IsA<UK2Node_CallFunction>() && BindFunction && UnbindFunction)
	{
		UK2Node_CallFunction* Node = Cast<UK2Node_CallFunction>(InPin->GetOuter());
		UFunction* CallingFunction = Node->GetTargetFunction();
		if (CallingFunction == BindFunction || CallingFunction == UnbindFunction)
		{
			UEdGraphPin* Pin = Node->FindPin(UEdGraphSchema_K2::PN_Self, EEdGraphPinDirection::EGPD_Input);
			if (Pin && Pin->LinkedTo.Num() > 0)
			{
				UEdGraphPin* OwnerPin = Pin->LinkedTo[0];
				if (OwnerPin->PinType.PinSubCategoryObject.IsValid())
				{
					UClass* TargetClass = Cast<UClass>(OwnerPin->PinType.PinSubCategoryObject.Get());
					if (TargetClass)
					{
						return SNew(SPsGraphPinEventList, InPin, GenerateEvents(TargetClass));
					}
				}
			}
			
		}
	}

	return nullptr;
}
