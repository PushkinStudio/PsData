// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsDataNode_Property.h"
#include "PsDataNode_Link.generated.h"

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_Link : public UPsDataNode_Property
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	FString Path;

	UPROPERTY()
	UClass* ReturnType;

	UPROPERTY()
	bool bCollection;

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodePure() const override { return true; }
	virtual FText GetMenuCategory() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End UK2Node interface.

	/** Create property pin */
	virtual UEdGraphPin* CreatePropertyPin(EEdGraphPinDirection Dir, bool bShowName = true) override;

	UEdGraphPin* GetInputPin();
	UEdGraphPin* GetOutputPin();

	void SetLinkPath(const FString& LinkPath);
};
