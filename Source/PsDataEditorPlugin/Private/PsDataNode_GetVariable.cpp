// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataNode_GetVariable.h"

#include "PsDataAPI.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"

void UPsDataNode_GetVariable::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

FText UPsDataNode_GetVariable::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_GetVariable::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad PsDataNode_GetVariable node"));
	}

	if (TitleType == ENodeTitleType::FullTitle)
	{
		return FText::FromString(FString::Printf(TEXT("Get %s::%s"), *TargetClass->GetName(), *PropertyName));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("Get %s"), *PropertyName));
	}
}

void UPsDataNode_GetVariable::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass, const FDataField& Field)
		{
			if (FDataReflection::GetFieldByName(TargetClass->GetSuperClass(), Field.Name).IsValid())
			{
				return nullptr;
			}

			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);

			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, TargetClass](UEdGraphNode* EvaluatorNode, bool) {
				UPsDataNode_Variable* Node = CastChecked<UPsDataNode_Variable>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field.Name;
				Node->PropertyCppType = Field.Context->GetCppType();
				Node->UpdateFunctionReference();
			});

			NodeSpawner->DefaultMenuSignature.Category = FText::FromString(FString::Printf(TEXT("PsData|%s"), *TargetClass->GetName()));
			return NodeSpawner;
		}
	};

	UClass* NodeClass = GetClass();
	for (UClass* Class : TObjectRange<UClass>())
	{
		for (auto& Pair : FDataReflection::GetFields(Class))
		{
			if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, *Pair.Value.Get()))
			{
				ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
			}
		}
	}
}

void UPsDataNode_GetVariable::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin)
{
	Super::UpdatePin(PinType, Pin);
	if (PinType == EPsDataVariablePinType::ReturnValue)
	{
		Pin->PinFriendlyName = FText::FromString(PropertyName);
	}
}

void UPsDataNode_GetVariable::UpdateFunctionReference()
{
	auto Field = GetProperty();
	if (!Field.IsValid())
	{
		return;
	}

	SetFromFunction(Field->Context->GetUFunctions()->GetFunction);
}
