// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataAPI.h"

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "Textures/SlateIcon.h"

#include "PsDataNode_Variable.generated.h"

enum class EPsDataVariablePinType : uint8
{
	Target,
	Index,
	PropertyIn,
	OldPropertyOut,
	PropertyOut,
	Unknown
};

class FNodeHandlingFunctor;

UCLASS()
class PSDATAEDITOR_API UPsDataNode_Variable : public UK2Node_CallFunction
{
	GENERATED_UCLASS_BODY()

protected:
	static const FName MD_PsDataTarget;
	static const FName MD_PsDataIndex;
	static const FName MD_PsDataOut;
	static const FName MD_PsDataIn;

	static const FName Default_TargetParam;
	static const FName Default_IndexParam;
	static const FName Default_OutParam;
	static const FName Default_InParam;

public:
	UPROPERTY()
	UClass* TargetClass;

	UPROPERTY()
	FString PropertyName;

	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual UK2Node::ERedirectType DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const override;
	virtual FText GetMenuCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool IsDeprecated() const override;
	// End UEdGraphNode interface

	// Begin UK2Node interface
	virtual bool DrawNodeAsVariable() const override { return true; }
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	virtual FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	// End UK2Node interface.

	/** Get property */
	const FDataField* GetProperty() const;

	/** Update allocated pin */
	virtual void UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin) const;

	/** Update allocated pin */
	virtual void UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const;

	/** Get function to call */
	virtual UFunction* GetFunction() const;

	/** Update function reference */
	void UpdateFunctionReference();
};
