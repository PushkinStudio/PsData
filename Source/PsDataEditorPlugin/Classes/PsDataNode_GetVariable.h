// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataNode_Variable.h"

#include "CoreMinimal.h"

#include "PsDataNode_GetVariable.generated.h"

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_GetVariable : public UPsDataNode_Variable
{
	GENERATED_BODY()

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodePure() const override { return true; }
	virtual FText GetMenuCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End UK2Node interface.

	/** Update allocated pin */
	virtual void UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin) override;

	/** Update function reference */
	virtual void UpdateFunctionReference() override;
};
