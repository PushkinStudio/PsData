// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "KismetCompilerMisc.h"

class FKismetCompilerContext;
struct FKismetFunctionContext;

class FPsDataHandler_CallFunction : public FNodeHandlingFunctor
{
public:
	FPsDataHandler_CallFunction(FKismetCompilerContext& InCompilerContext);

	virtual void RegisterNet(FKismetFunctionContext& Context, UEdGraphPin* Net) override;
	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override;

private:
	bool IsCalledFunctionPure(UEdGraphNode* Node) const;
	FName GetFunctionNameFromNode(UEdGraphNode* Node) const;
	UClass* GetCallingContext(FKismetFunctionContext& Context, UEdGraphNode* Node) const;
	UFunction* FindFunction(FKismetFunctionContext& Context, UEdGraphNode* Node) const;
	void CreateFunctionCallStatement(FKismetFunctionContext& Context, UEdGraphNode* Node, UEdGraphPin* SelfPin);
};
