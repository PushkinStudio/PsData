// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Pins/PsDataPinFactory.h"

#include "Pins/PsDataGraphPinEventList.h"
#include "PsDataAPI.h"

#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "SGraphPin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

TArray<TSharedPtr<FEventPath>> GenerateEvents(UClass* TargetClass)
{
	struct FHelperStruct
	{
	public:
		UField* Field;
		FString Path;

		FHelperStruct(UField* InField, const FString& InPath)
			: Field(InField)
			, Path(InPath)
		{
		}
	};

	TArray<FHelperStruct> Children;
	TArray<TSharedPtr<FEventPath>> Events;

	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::Added, TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::AddedToParent, TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::AddedToRoot, TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::Removed, TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::RemovedFromParent, TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::RemovedFromRoot, TEXT(""), true)));
	Events.Add(MakeShareable(new FEventPath(UPsDataEvent::Changed, TEXT(""), true)));

	for (const auto Field : PsDataTools::FDataReflection::GetFieldsByClass(TargetClass)->GetFieldsList())
	{
		if (Field->Meta.bEvent)
		{
			Events.Add(MakeShareable(new FEventPath(Field->GetChangedEventName(), TEXT(""), true)));
		}

		if (Field->Context->IsData())
		{
			Children.Add(FHelperStruct(Field->Context->GetUE4Type(), Field->Name));
		}
	}

	while (Children.Num() > 0)
	{
		FHelperStruct Child = Children.Last();
		Children.RemoveAt(Children.Num() - 1, 1, false);

		UClass* Class = Cast<UClass>(Child.Field);
		if (Class == nullptr)
		{
			continue;
		}

		for (const auto Field : PsDataTools::FDataReflection::GetFieldsByClass(Class)->GetFieldsList())
		{
			FString Path = Child.Path + TEXT(".") + Field->Name;
			if (Field->Meta.bEvent && Field->Meta.bBubbles)
			{
				Events.Add(MakeShareable(new FEventPath(Field->GetChangedEventName(), Path, false)));
			}

			if (Field->Context->IsData())
			{
				Children.Add(FHelperStruct(Field->Context->GetUE4Type(), Path));
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
						return SNew(SPsDataGraphPinEventList, InPin, GenerateEvents(TargetClass));
					}
				}
			}
		}
	}

	return nullptr;
}
