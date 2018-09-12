// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "PsDataNode_GetProperty.h"
#include "PsDataAPI.h"

#include "KismetCompiler.h"
#include "EditorCategoryUtils.h"
#include "EdGraphUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#define LOCTEXT_NAMESPACE "PsDataNode_GetProperty"

class FKCHandler_GetProperty : public FNodeHandlingFunctor
{
public:
	FKCHandler_GetProperty(FKismetCompilerContext& InCompilerContext)
	: FNodeHandlingFunctor(InCompilerContext)
	{
	}
	
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_GetProperty* PropNode = CastChecked<UPsDataNode_GetProperty>(Node);
		if (PropNode->HasError())
		{
			Context.MessageLog.Error(TEXT("Bad Node"));
			return;
		}
		
		UEdGraphPin* OutputPin = PropNode->GetOutputPin();
		if (OutputPin->LinkedTo.Num() == 0)
		{
			return;
		}
		
		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputPin());
		FBPTerminal** TargetTerm = Context.NetMap.Find(InputTargetNet);
		if (TargetTerm == nullptr)
		{
			return;
		}
		
		{
			FString FunctionName = FDataReflection::GenerateGetFunctionName(*PropNode->GetProperty());
			UFunction* Function = UPsData::StaticClass()->FindFunctionByName(FName(*FunctionName));
			if (Function == nullptr)
			{
				Context.MessageLog.Error(*FString::Printf(TEXT("Function \"%s\" not found"), *FunctionName));
				return;
			}
			
			// Create get statment
			FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
			Statement.Type = KCST_CallFunction;
			Statement.FunctionToCall = Function;
			Statement.FunctionContext = *TargetTerm;
			Statement.bIsParentContext = false;
			
			// First argument
			FBPTerminal* ArgFieldNameTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
			ArgFieldNameTerm->Type.PinCategory = CompilerContext.GetSchema()->PC_String;
			ArgFieldNameTerm->Name = PropNode->PropertyName;
			ArgFieldNameTerm->TextLiteral = FText::FromString(PropNode->PropertyName);
			Statement.RHS.Add(ArgFieldNameTerm);
			
			// Result
			Statement.LHS = *Context.NetMap.Find(OutputPin);
		}
	}
	
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_GetProperty* PropNode = CastChecked<UPsDataNode_GetProperty>(Node);
		FNodeHandlingFunctor::RegisterNets(Context, PropNode);
		
		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputPin());
		if (!Context.NetMap.Contains(InputTargetNet))
		{
			Context.NetMap.Add(InputTargetNet, Context.CreateLocalTerminalFromPinAutoChooseScope(InputTargetNet, Context.NetNameMap->MakeValidName(InputTargetNet)));
		}
		
		UEdGraphPin* OutputPin = PropNode->GetOutputPin();
		FBPTerminal* OutputTerm = Context.CreateLocalTerminalFromPinAutoChooseScope(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
		Context.NetMap.Add(OutputPin, OutputTerm);
	}
	
};

void UPsDataNode_GetProperty::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	CreateTargetPin(EGPD_Input);
	CreatePropertyPin(EGPD_Output);
}

FText UPsDataNode_GetProperty::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_GetProperty::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsData_GetProperty node"));
	}
	
	switch (TitleType) {
		case ENodeTitleType::MenuTitle:
			return FText::FromString(FString::Printf(TEXT("Get %s"), *PropertyName));
		case ENodeTitleType::FullTitle:
			return FText::FromString(FString::Printf(TEXT("Get %s::%s"), *TargetClass->GetName(), *PropertyName));
		default:
			return FText::FromString(TEXT("Get"));
	}
}

FNodeHandlingFunctor* UPsDataNode_GetProperty::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_GetProperty(CompilerContext);
}

UEdGraphPin* UPsDataNode_GetProperty::GetInputPin()
{
	UEdGraphPin* InputPin = NULL;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Input)
		{
			return Pin;
		}
	}
	
	return nullptr;
}

UEdGraphPin* UPsDataNode_GetProperty::GetOutputPin()
{
	UEdGraphPin* InputPin = NULL;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output)
		{
			return Pin;
		}
	}
	
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
