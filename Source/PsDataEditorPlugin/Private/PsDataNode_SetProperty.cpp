// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataNode_SetProperty.h"
#include "PsDataAPI.h"

#include "KismetCompiler.h"
#include "EditorCategoryUtils.h"
#include "EdGraphUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_SetProperty"

class FKCHandler_SetProperty : public FNodeHandlingFunctor
{
public:
	FKCHandler_SetProperty(FKismetCompilerContext& InCompilerContext)
	: FNodeHandlingFunctor(InCompilerContext)
	{
	}
	
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_SetProperty* PropNode = CastChecked<UPsDataNode_SetProperty>(Node);
		if (PropNode->HasError())
		{
			Context.MessageLog.Error(TEXT("Bad Node"));
			return;
		}
		
		UEdGraphPin* ExecInputPin = PropNode->GetExecInputPin();
		if (ExecInputPin->LinkedTo.Num() == 0)
		{
			CompilerContext.MessageLog.Warning(*NSLOCTEXT("PsData", "NodeNeverExecuted_Warning", "@@ will never be executed").ToString(), Node);
			return;
		}
		
		UEdGraphPin* InputTargetPin = PropNode->GetInputTargetPin();
		if (InputTargetPin->LinkedTo.Num() == 0)
		{
			CompilerContext.MessageLog.Error(*NSLOCTEXT("PsData", "PinMustHaveConnectionPruned_Error", "Pin @@ must have a connection.").ToString(), InputTargetPin);
			return;
		}
		
		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(InputTargetPin);
		FBPTerminal** TargetTerm = Context.NetMap.Find(InputTargetNet);
		if (TargetTerm == nullptr)
		{
			return;
		}
		
		{
			FString FunctionName = FDataReflection::GenerateSetFunctionName(*PropNode->GetProperty());
			UFunction* Function = UPsData::StaticClass()->FindFunctionByName(FName(*FunctionName));
			if (Function == nullptr)
			{
				Context.MessageLog.Error(*FString::Printf(TEXT("Function \"%s\" not found"), *FunctionName));
				return;
			}
			
			// Create set statment
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
			
			// Second argument
			UEdGraphPin* InputPropertyPin = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputPropertyPin());
			FBPTerminal* ArgValueTerm = *Context.NetMap.Find(InputPropertyPin);
			Statement.RHS.Add(ArgValueTerm);
		}
		
		
		UEdGraphPin* OutputPropertyPin = PropNode->GetOutputPropertyPin();
		if (OutputPropertyPin->LinkedTo.Num() != 0)
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
			Statement.LHS = *Context.NetMap.Find(OutputPropertyPin);
		}
		
		
	}
	
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_SetProperty* PropNode = CastChecked<UPsDataNode_SetProperty>(Node);
		FNodeHandlingFunctor::RegisterNets(Context, PropNode);
		
		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputTargetPin());
		if (!Context.NetMap.Contains(InputTargetNet))
		{
			Context.NetMap.Add(InputTargetNet, Context.CreateLocalTerminalFromPinAutoChooseScope(InputTargetNet, Context.NetNameMap->MakeValidName(InputTargetNet)));
		}
		
		UEdGraphPin* InputPropertyNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputPropertyPin());
		if (!Context.NetMap.Contains(InputPropertyNet))
		{
			Context.NetMap.Add(InputPropertyNet, Context.CreateLocalTerminalFromPinAutoChooseScope(InputPropertyNet, Context.NetNameMap->MakeValidName(InputPropertyNet)));
		}
		
		UEdGraphPin* OutputPropertyPin = PropNode->GetOutputPropertyPin();
		if (OutputPropertyPin->LinkedTo.Num() != 0)
		{
			FBPTerminal* OutputPropertyTerm = Context.CreateLocalTerminalFromPinAutoChooseScope(OutputPropertyPin, Context.NetNameMap->MakeValidName(OutputPropertyPin));
			Context.NetMap.Add(OutputPropertyPin, OutputPropertyTerm);
		}
		
	}
	
};

void UPsDataNode_SetProperty::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	CreateExecPin();
	CreatePropertyPin(EGPD_Input);
	CreatePropertyPin(EGPD_Output, false);
	CreateTargetPin(EGPD_Input);
}

FText UPsDataNode_SetProperty::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_SetProperty::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_SetProperty node"));
	}
	
	switch (TitleType) {
		case ENodeTitleType::MenuTitle:
			return FText::FromString(FString::Printf(TEXT("Set %s"), *PropertyName));
		case ENodeTitleType::FullTitle:
			return FText::FromString(FString::Printf(TEXT("Set %s::%s"), *TargetClass->GetName(), *PropertyName));
		default:
			return FText::FromString(TEXT("Set"));
	}
}

FNodeHandlingFunctor* UPsDataNode_SetProperty::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_SetProperty(CompilerContext);
}

UEdGraphPin* UPsDataNode_SetProperty::GetExecInputPin()
{
	return Pins[0];
}

UEdGraphPin* UPsDataNode_SetProperty::GetExexOutputPin()
{
	return Pins[1];
}

UEdGraphPin* UPsDataNode_SetProperty::GetInputTargetPin()
{
	return Pins[4];
}

UEdGraphPin* UPsDataNode_SetProperty::GetInputPropertyPin()
{
	return Pins[2];
}

UEdGraphPin* UPsDataNode_SetProperty::GetOutputPropertyPin()
{
	return Pins[3];
}

#undef LOCTEXT_NAMESPACE
