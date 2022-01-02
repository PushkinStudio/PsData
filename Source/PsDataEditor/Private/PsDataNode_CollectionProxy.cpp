// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataNode_CollectionProxy.h"

#include "PsDataAPI.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_CollectionProxy"

UPsDataNode_CollectionProxy::UPsDataNode_CollectionProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UPsDataNode_CollectionProxy::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FLinearColor UPsDataNode_CollectionProxy::GetNodeTitleColor() const
{
	return UK2Node_CallFunction::GetNodeTitleColor();
}

FSlateIcon UPsDataNode_CollectionProxy::GetIconAndTint(FLinearColor& ColorOut) const
{
	return UK2Node_CallFunction::GetIconAndTint(ColorOut);
}

FText UPsDataNode_CollectionProxy::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_CollectionProxy node"));
	}

	if (TitleType == ENodeTitleType::FullTitle)
	{
		return FText::FromString(FString::Printf(TEXT("Get %s::%s Proxy"), *TargetClass->GetName(), *PropertyName));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("Get %s Proxy"), *PropertyName));
	}
}

void UPsDataNode_CollectionProxy::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass, const FDataField* Field)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);

			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, TargetClass](UEdGraphNode* EvaluatorNode, bool) {
				UPsDataNode_CollectionProxy* Node = CastChecked<UPsDataNode_CollectionProxy>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field->Name;
				Node->UpdateFunctionReference();
			});

			NodeSpawner->DefaultMenuSignature.Category = FText::FromString(FString::Printf(TEXT("PsData|%s"), *TargetClass->GetName()));

			return NodeSpawner;
		}
	};

	for (UClass* Class : TObjectRange<UClass>())
	{
		for (const auto Field : PsDataTools::FDataReflection::GetFieldsByClass(Class)->GetFieldsList())
		{
			if (Field->Context->IsData() && Field->Context->IsContainer())
			{
				if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, Field))
				{
					ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
				}
			}
		}
	}
}

void UPsDataNode_CollectionProxy::UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const
{
	Super::UpdatePinByContext(PinType, Pin, Index, Context);
	if (PinType == EPsDataVariablePinType::PropertyOut || PinType == EPsDataVariablePinType::OldPropertyOut)
	{
		Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("%s Proxy"), *PropertyName));
	}
}

UFunction* UPsDataNode_CollectionProxy::GetFunction() const
{
	const auto Field = GetProperty();
	if (!Field)
	{
		return nullptr;
	}

	const FString FunctionName = (Field->Context->IsArray() ? TEXT("GetArrayProxy") : TEXT("GetMapProxy"));
	UFunction* Function = UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));
	return Function;
}

#undef LOCTEXT_NAMESPACE
