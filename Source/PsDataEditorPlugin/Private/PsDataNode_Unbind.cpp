// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "PsDataNode_Unbind.h"
#include "PsData.h"

#include "KismetCompiler.h"
#include "EditorCategoryUtils.h"
#include "EdGraphUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "Engine/MemberReference.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_Unbind"

class FKCHandler_Unbind : public FNodeHandlingFunctor
{
public:
	FKCHandler_Unbind(FKismetCompilerContext& InCompilerContext)
	: FNodeHandlingFunctor(InCompilerContext)
	{
	}
	
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_Unbind* PropNode = CastChecked<UPsDataNode_Unbind>(Node);
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
			FString FunctionName(TEXT("BlueprintUnbind"));
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
		UPsDataNode_Unbind* PropNode = CastChecked<UPsDataNode_Unbind>(Node);
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

void UPsDataNode_Unbind::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	CreateExecPin();
	CreateTargetPin();
	CreatePins();
}

FText UPsDataNode_Unbind::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_Unbind::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_Unbind node"));
	}
	
	return FText::FromString(FString::Printf(TEXT("Unbind Event For %s"), *TargetClass->GetName()));
}

FNodeHandlingFunctor* UPsDataNode_Unbind::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_Unbind(CompilerContext);
}

void UPsDataNode_Unbind::CreatePins()
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

UEdGraphPin* UPsDataNode_Unbind::GetExecInputPin()
{
	return Pins[0];
}

UEdGraphPin* UPsDataNode_Unbind::GetExexOutputPin()
{
	return Pins[1];
}

UEdGraphPin* UPsDataNode_Unbind::GetInputTargetPin()
{
	return Pins[2];
}

UEdGraphPin* UPsDataNode_Unbind::GetInputNamePin()
{
	return Pins[3];
}

UEdGraphPin* UPsDataNode_Unbind::GetInputCallbackPin()
{
	return Pins[4];
}

#undef LOCTEXT_NAMESPACE

