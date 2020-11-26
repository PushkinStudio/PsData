// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "KismetCompilerMisc.h"

class FKismetCompilerContext;
struct FKismetFunctionContext;

//////////////////////////////////////////////////////////////////////////
// PsDataHandler_CallFunction
// Based on vanilla UE CallFunctionHandler with type checking modifications
// Original code Copyright Epic Games, Inc.

class FPsDataHandler_CallFunction : public FNodeHandlingFunctor
{
public:
	FPsDataHandler_CallFunction(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	void CreateFunctionCallStatement(FKismetFunctionContext& Context, UEdGraphNode* Node, UEdGraphPin* SelfPin);

	bool IsCalledFunctionPure(UEdGraphNode* Node)
	{
		if (UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(Node))
		{
			return CallFunctionNode->bIsPureFunc;
		}
		return false;
	}

	bool IsCalledFunctionFinal(UEdGraphNode* Node)
	{
		if (UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(Node))
		{
			return CallFunctionNode->bIsFinalFunction;
		}
		return false;
	}

	bool IsCalledFunctionFromInterface(UEdGraphNode* Node)
	{
		if (UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(Node))
		{
			return CallFunctionNode->bIsInterfaceCall;
		}
		return false;
	}

private:
	virtual FName GetFunctionNameFromNode(UEdGraphNode* Node) const;
	UClass* GetCallingContext(FKismetFunctionContext& Context, UEdGraphNode* Node);
	UClass* GetTrueCallingClass(FKismetFunctionContext& Context, UEdGraphPin* SelfPin);

	// Based on FKCPushModelHelpers::ConstructMarkDirtyNodeForProperty
	static UEdGraphNode* ConstructMarkDirtyNodeForProperty(struct FKismetFunctionContext& Context, class FProperty* RepProperty, class UEdGraphPin* PropertyObjectPin);

public:
	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override;
	virtual void RegisterNet(FKismetFunctionContext& Context, UEdGraphPin* Net) override;
	virtual UFunction* FindFunction(FKismetFunctionContext& Context, UEdGraphNode* Node);
	virtual void Transform(FKismetFunctionContext& Context, UEdGraphNode* Node) override;
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override;
	virtual void CheckIfFunctionIsCallable(UFunction* Function, FKismetFunctionContext& Context, UEdGraphNode* Node);
	virtual void AdditionalCompiledStatementHandling(FKismetFunctionContext& Context, UEdGraphNode* Node, FBlueprintCompiledStatement& Statement) {}

protected:
	TMap<UEdGraphPin*, FBPTerminal*> InterfaceTermMap;
};
