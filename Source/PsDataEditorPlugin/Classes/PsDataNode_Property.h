// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "PsDataAPI.h"
#include "Textures/SlateIcon.h"
#include "PsDataNode_Property.generated.h"

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_Property : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	UClass* TargetClass;
	
	UPROPERTY()
	FString PropertyName;
	
protected:
	/** Unsupported type flag */
	bool bUnsupportedType;

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool IsDeprecated() const override;
	// End UEdGraphNode interface.
	
	// Begin UK2Node interface
	virtual bool DrawNodeAsVariable() const override { return true; }
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End UK2Node interface.
	
	/** Create exec pin */
	void CreateExecPin();
	
	/** Create target property pin */
	virtual UEdGraphPin* CreateTargetPin(EEdGraphPinDirection Dir, bool bShowName = true);
	
	/** Create property pin */
	virtual UEdGraphPin* CreatePropertyPin(EEdGraphPinDirection Dir, bool bShowName = true);
	
	/** Has error */
	bool HasError() const;
	
	/** Get property */
	const FDataFieldDescription* GetProperty() const;
};
