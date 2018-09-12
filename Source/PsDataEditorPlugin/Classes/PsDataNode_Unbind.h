// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsDataNode_Event.h"
#include "PsDataNode_Unbind.generated.h"

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_Unbind : public UPsDataNode_Event
{
	GENERATED_BODY()
	
public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	// End UEdGraphNode interface.
	
	// Begin UK2Node interface
	virtual FText GetMenuCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	// End UK2Node interface.
	
	/** Create input pins */
	void CreatePins();
	
	UEdGraphPin* GetExecInputPin();
	UEdGraphPin* GetExexOutputPin();
	UEdGraphPin* GetInputTargetPin();
	UEdGraphPin* GetInputNamePin();
	UEdGraphPin* GetInputCallbackPin();
	
};
