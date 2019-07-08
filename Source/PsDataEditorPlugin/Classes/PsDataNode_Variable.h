// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataAPI.h"

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "Textures/SlateIcon.h"

#include "PsDataNode_Variable.generated.h"

enum class EPsDataVariablePinType : uint8
{
	Target,
	PropertyHash,
	Value,
	ReturnValue,
	Unknown
};

UCLASS()
class PSDATAEDITORPLUGIN_API UPsDataNode_Variable : public UK2Node_CallFunction
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	UClass* TargetClass;

	UPROPERTY()
	FString PropertyName;

	UPROPERTY()
	FString PropertyCppType;

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetMenuCategory() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool IsDeprecated() const override;
	// End UEdGraphNode interface

	// Begin UK2Node interface
	virtual bool DrawNodeAsVariable() const override { return true; }
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	// End UK2Node interface.

	/** Get property */
	TSharedPtr<const FDataField> GetProperty() const;

	/** Update allocated pin */
	virtual void UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin);

	/** Update function reference */
	virtual void UpdateFunctionReference();
};
