// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataAPI.h"
#include "CoreMinimal.h"
#include "K2Node.h"
#include "Textures/SlateIcon.h"
#include "PsDataNode_Event.generated.h"

struct FEventPath
{
	/** Event type */
	FString Type;
	
	/** Event path */
	FString Path;
	
	/** Self */
	bool bSelf;
	
	FEventPath(FString InType, FString InPath, bool bInSelf)
	: Type(InType)
	, Path(InPath)
	, bSelf(bInSelf)
	{}
};

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_Event : public UK2Node
{
	GENERATED_UCLASS_BODY()
	
public:
	/** Target class */
	UPROPERTY()
	UClass* TargetClass;
	
	/** Events */
	TArray<FEventPath> Events;
	
public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	// End UEdGraphNode interface.
	
	// Begin UK2Node interface
	virtual bool IsNodePure() const override { return false; }
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// End UK2Node interface.
	
	/** Create exec pin */
	void CreateExecPin();
	
	/** Create target pin */
	void CreateTargetPin();
	
	/** Has error */
	bool HasError() const;
	
	/** Generate event */
	void GenerateEvents();
};
