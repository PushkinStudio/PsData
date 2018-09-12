// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "PsDataNode_Bind.h"
#include "PsData.h"

#include "KismetCompiler.h"
#include "EditorCategoryUtils.h"
#include "EdGraphUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Engine/MemberReference.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_Bind"

class FKCHandler_Bind : public FNodeHandlingFunctor
{
public:
	FKCHandler_Bind(FKismetCompilerContext& InCompilerContext)
	: FNodeHandlingFunctor(InCompilerContext)
	{
	}
	
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_Bind* PropNode = CastChecked<UPsDataNode_Bind>(Node);
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
			FString FunctionName(TEXT("BlueprintBind"));
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
			UEdGraphPin* InputNamePin = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputNamePin());
			FBPTerminal* ArgNameTerm = *Context.NetMap.Find(InputNamePin);
			Statement.RHS.Add(ArgNameTerm);
			
			// Second argument
			UEdGraphPin* InputCallbackPin = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputCallbackPin());
			FBPTerminal* ArgCallbackTerm = *Context.NetMap.Find(InputCallbackPin);
			Statement.RHS.Add(ArgCallbackTerm);
			
		}
	}
	
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_Bind* PropNode = CastChecked<UPsDataNode_Bind>(Node);
		FNodeHandlingFunctor::RegisterNets(Context, PropNode);
		
		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputTargetPin());
		if (!Context.NetMap.Contains(InputTargetNet))
		{
			Context.NetMap.Add(InputTargetNet, Context.CreateLocalTerminalFromPinAutoChooseScope(InputTargetNet, Context.NetNameMap->MakeValidName(InputTargetNet)));
		}
		
		UEdGraphPin* InputNameNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputNamePin());
		if (!Context.NetMap.Contains(InputNameNet))
		{
			Context.NetMap.Add(InputNameNet, Context.CreateLocalTerminalFromPinAutoChooseScope(InputNameNet, Context.NetNameMap->MakeValidName(InputNameNet)));
		}
		
		UEdGraphPin* InputCallbackNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputCallbackPin());
		if (!Context.NetMap.Contains(InputCallbackNet))
		{
			FBPTerminal* CallbackTerm = Context.CreateLocalTerminalFromPinAutoChooseScope(InputCallbackNet, Context.NetNameMap->MakeValidName(InputCallbackNet));
			Context.NetMap.Add(InputCallbackNet, CallbackTerm);
		}
		
		
		{
			FBPTerminal* CallbackTerm = *Context.NetMap.Find(InputCallbackNet);
			if (FMemberReference::ResolveSimpleMemberReference<UFunction>(CallbackTerm->Type.PinSubCategoryMemberReference) == nullptr)
			{
				FMemberReference::FillSimpleMemberReference<UFunction>(FMemberReference::ResolveSimpleMemberReference<UFunction>(PropNode->GetInputCallbackPin()->PinType.PinSubCategoryMemberReference), CallbackTerm->Type.PinSubCategoryMemberReference);
			}
			
			if (FMemberReference::ResolveSimpleMemberReference<UFunction>(CallbackTerm->Type.PinSubCategoryMemberReference) == nullptr)
			{
				CompilerContext.MessageLog.Error(*LOCTEXT("UnconnectedDelegateSig", "Event Dispatcher has no signature @@").ToString(), PropNode->GetInputCallbackPin());
				return;
			}
		}
		
	}
	
};

void UPsDataNode_Bind::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	CreateExecPin();
	CreateTargetPin();
	CreatePins();
}

FText UPsDataNode_Bind::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_Bind::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_Bind node"));
	}
	
	return FText::FromString(FString::Printf(TEXT("Bind Event For %s"), *TargetClass->GetName()));
}

FNodeHandlingFunctor* UPsDataNode_Bind::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_Bind(CompilerContext);
}

void UPsDataNode_Bind::CreatePins()
{
	UProperty* Property = nullptr;
	UFunction* Function = UPsData::StaticClass()->FindFunctionByName(TEXT("BlueprintUnbind"), EIncludeSuperFlag::IncludeSuper);
	for (TFieldIterator<UProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		Property = *PropIt;
		UEdGraphPin* Pin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, *Property->GetName());
		GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Property, Pin->PinType);
	}
}

UEdGraphPin* UPsDataNode_Bind::GetExecInputPin()
{
	return Pins[0];
}

UEdGraphPin* UPsDataNode_Bind::GetExexOutputPin()
{
	return Pins[1];
}

UEdGraphPin* UPsDataNode_Bind::GetInputTargetPin()
{
	return Pins[2];
}

UEdGraphPin* UPsDataNode_Bind::GetInputNamePin()
{
	return Pins[3];
}

UEdGraphPin* UPsDataNode_Bind::GetInputCallbackPin()
{
	return Pins[4];
}

#undef LOCTEXT_NAMESPACE
