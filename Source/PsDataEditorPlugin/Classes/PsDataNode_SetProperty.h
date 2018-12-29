// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsDataNode_Property.h"

#include "PsDataNode_SetProperty.generated.h"

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_SetProperty : public UPsDataNode_Property
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
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	// End UK2Node interface.
	
	UEdGraphPin* GetExecInputPin();
	UEdGraphPin* GetExexOutputPin();
	UEdGraphPin* GetInputTargetPin();
	UEdGraphPin* GetInputPropertyPin();
	UEdGraphPin* GetOutputPropertyPin();
	
};
