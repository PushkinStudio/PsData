// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataNode_Variable.h"

#include "CoreMinimal.h"

#include "PsDataNode_SetVariable.generated.h"

UCLASS()
class PSDATAEDITOR_API UPsDataNode_SetVariable : public UPsDataNode_Variable
{
	GENERATED_BODY()

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodePure() const override { return false; }
	virtual FText GetMenuCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End UK2Node interface.

	/** Update allocated pin */
	virtual void UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const override;

	/** Get function to call */
	virtual UFunction* GetFunction() const override;
};
