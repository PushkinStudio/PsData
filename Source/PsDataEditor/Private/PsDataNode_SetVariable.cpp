// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataNode_SetVariable.h"

#include "PsDataAPI.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_SetVariable"

void UPsDataNode_SetVariable::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

FText UPsDataNode_SetVariable::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_SetVariable::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad PsDataNode_SetVariable node"));
	}

	if (TitleType == ENodeTitleType::FullTitle)
	{
		return FText::FromString(FString::Printf(TEXT("Set %s::%s"), *TargetClass->GetName(), *PropertyName));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("Set %s"), *PropertyName));
	}
}

void UPsDataNode_SetVariable::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
	const auto Field = GetProperty();
	if (Field)
	{
		if (Field->Meta.bStrict || Field->Meta.bReadOnly)
		{
			MessageLog.Error(*LOCTEXT("UPsDataNode_SetVariable", "Node @@: unsupported for read only property").ToString(), this);
		}
	}
}

void UPsDataNode_SetVariable::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass, const FDataField* Field)
		{
			if (Field->Meta.bStrict || Field->Meta.bReadOnly)
			{
				return nullptr;
			}

			if (PsDataTools::FDataReflection::GetFieldsByClass(TargetClass->GetSuperClass())->HasFieldWithName(Field->Name))
			{
				return nullptr;
			}

			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);

			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, TargetClass](UEdGraphNode* EvaluatorNode, bool) {
				UPsDataNode_Variable* Node = CastChecked<UPsDataNode_Variable>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field->Name;
				Node->UpdateFunctionReference();
			});

			NodeSpawner->DefaultMenuSignature.Category = FText::FromString(FString::Printf(TEXT("PsData|%s"), *TargetClass->GetName()));
			return NodeSpawner;
		}
	};

	UClass* NodeClass = GetClass();
	for (UClass* Class : TObjectRange<UClass>())
	{
		for (const auto Field : PsDataTools::FDataReflection::GetFieldsByClass(Class)->GetFieldsList())
		{
			if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, Field))
			{
				ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
			}
		}
	}
}

void UPsDataNode_SetVariable::UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const
{
	Super::UpdatePinByContext(PinType, Pin, Index, Context);
	if (PinType == EPsDataVariablePinType::PropertyOut || PinType == EPsDataVariablePinType::OldPropertyOut)
	{
		Pin->PinFriendlyName = FText::FromString(TEXT(""));
	}
}

UFunction* UPsDataNode_SetVariable::GetFunction() const
{
	const auto Field = GetProperty();
	if (!Field)
	{
		return nullptr;
	}

	return Field->Context->GetUFunctions().ResolveSetFunction();
}

#undef LOCTEXT_NAMESPACE
