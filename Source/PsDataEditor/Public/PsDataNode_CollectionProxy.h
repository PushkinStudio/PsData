// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataNode_Variable.h"

#include "CoreMinimal.h"

#include "PsDataNode_CollectionProxy.generated.h"

UCLASS()
class PSDATAEDITOR_API UPsDataNode_CollectionProxy : public UPsDataNode_Variable
{
	GENERATED_UCLASS_BODY()

public:
	// Begin UK2Node interface
	virtual bool IsNodePure() const override { return true; }
	virtual FText GetMenuCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End UK2Node interface.

	/** Update allocated pin */
	virtual void UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const override;

	/** Get function to call */
	virtual UFunction* GetFunction() const override;
};
