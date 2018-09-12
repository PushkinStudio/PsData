// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "PsDataNode_Event.h"
#include "BlueprintNodeSpawner.h"
#include "Styling/SlateIconFinder.h"
#include "BlueprintActionDatabaseRegistrar.h"

UPsDataNode_Event::UPsDataNode_Event(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

void UPsDataNode_Event::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	GenerateEvents();
}

FLinearColor UPsDataNode_Event::GetNodeTitleColor() const
{
	if (HasError())
	{
		return FLinearColor::White;
	}
	return FLinearColor(0.f, 1.f, 0.f, 1.f);
}

FSlateIcon UPsDataNode_Event::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = Super::GetNodeTitleColor();
	return FSlateIcon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
}

void UPsDataNode_Event::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);
			
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([TargetClass](UEdGraphNode* EvaluatorNode, bool)
			{
				UPsDataNode_Event* Node = CastChecked<UPsDataNode_Event>(EvaluatorNode);
				Node->TargetClass = TargetClass;
			});
			
			NodeSpawner->DefaultMenuSignature.Category = FText::FromString(FString::Printf(TEXT("PsData|%s"), *TargetClass->GetName()));
			
			return NodeSpawner;
		}
	};

	UClass* NodeClass = GetClass();
	for (UClass* Class : TObjectRange<UClass>())
	{
		if (FDataReflection::HasClass(Class))
		{
			if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class))
			{
				ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
			}
		}
	}
}

void UPsDataNode_Event::CreateExecPin()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
}

void UPsDataNode_Event::CreateTargetPin()
{
	if (TargetClass == nullptr)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, TEXT("Target"));
	}
	else
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, NAME_None, TargetClass, TEXT("Target"));
	}
}

bool UPsDataNode_Event::HasError() const
{
	if (TargetClass == nullptr || !FDataReflection::HasClass(TargetClass))
	{
		return true;
	}
	
	return false;
}

void UPsDataNode_Event::GenerateEvents()
{
	struct FHelperStruct
	{
	public:
		UClass* Class;
		FString Path;
		
		FHelperStruct(UClass* InClass, const FString& InPath)
		: Class(InClass)
		, Path(InPath)
		{}
	};
	
	TArray<FHelperStruct> Children;
	Events.Reset();
	
	for (auto& Pair : FDataReflection::GetFields(TargetClass))
	{
		if (Pair.Value.Meta.bEvent)
		{
			Events.Add(FEventPath(FDataReflection::GenerateChangePropertyEventTypeName(Pair.Value), TEXT(""), true));
		}
		
		if (Pair.Value.Type == EDataFieldType::DFT_Data)
		{
			Children.Add(FHelperStruct(Pair.Value.Class, Pair.Value.Name));
		}
	}
	
	while(Children.Num() > 0)
	{
		FHelperStruct Child = Children.Last();
		Children.RemoveAt(Children.Num() - 1, 1, false);
		
		for (auto& Pair : FDataReflection::GetFields(Child.Class))
		{
			FString Path = Child.Path + TEXT(".") + Pair.Value.Name;
			if (Pair.Value.Meta.bEvent && Pair.Value.Meta.bBubbles)
			{
				Events.Add(FEventPath(FDataReflection::GenerateChangePropertyEventTypeName(Pair.Value), Path, false));
			}
			
			if (Pair.Value.Type == EDataFieldType::DFT_Data)
			{
				Children.Add(FHelperStruct(Pair.Value.Class, Path));
			}
			
		}
	}
}


