// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "Types/PsDataBigInteger.h"

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "Styling/SlateColor.h"

class IPropertyHandle;
class SEditableTextBox;

class PSDATAEDITOR_API FPsDataBigIntegerStructCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
	void SetBigIntegerValue(const FPsDataBigInteger& Value);

private:
	FSlateColor HandleTextBoxForegroundColor() const;
	FText HandleTextBoxText() const;
	void HandleTextBoxTextChanged(const FText& NewText);
	void HandleTextBoxTextCommited(const FText& NewText, ETextCommit::Type CommitInfo);

	bool bInputValid;
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TSharedPtr<SEditableTextBox> TextBox;
};
