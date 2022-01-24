// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Customization/PsDataBigIntegerStructCustomization.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "ScopedTransaction.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "FPsDataBigIntegerStructCustomization"

void WriteBigIntegerToProperty(TSharedPtr<IPropertyHandle> BigIntegerPropertyHandle, const FPsDataBigInteger& Value)
{
	FScopedTransaction Transaction(FText::Format(LOCTEXT("EditBigIntegerPropertyTransaction", "Edit {0}"), BigIntegerPropertyHandle->GetPropertyDisplayName()));

	TArray<void*> RawDatas;
	BigIntegerPropertyHandle->AccessRawData(RawDatas);
	BigIntegerPropertyHandle->NotifyPreChange();
	for (void* RawData : RawDatas)
	{
		auto Data = static_cast<FPsDataBigInteger*>(RawData);
		*Data = Value;
	}
	BigIntegerPropertyHandle->NotifyPostChange();
}

TSharedRef<IPropertyTypeCustomization> FPsDataBigIntegerStructCustomization::MakeInstance()
{
	return MakeShareable(new FPsDataBigIntegerStructCustomization);
}

void FPsDataBigIntegerStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;
	bInputValid = true;

	// clang-format off
	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(125.f)
		.MaxDesiredWidth(125.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SAssignNew(TextBox, SEditableTextBox)
						.ClearKeyboardFocusOnCommit(false)
						.IsEnabled(!PropertyHandle->IsEditConst())
						.ForegroundColor(this, &FPsDataBigIntegerStructCustomization::HandleTextBoxForegroundColor)
						.OnTextChanged(this, &FPsDataBigIntegerStructCustomization::HandleTextBoxTextChanged)
						.OnTextCommitted(this, &FPsDataBigIntegerStructCustomization::HandleTextBoxTextCommited)
						.SelectAllTextOnCommit(true)
						.Text(this, &FPsDataBigIntegerStructCustomization::HandleTextBoxText)
				]
		];
	// clang-format on
}

void FPsDataBigIntegerStructCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FPsDataBigIntegerStructCustomization::SetBigIntegerValue(const FPsDataBigInteger& Value)
{
	WriteBigIntegerToProperty(PropertyHandle, Value);
}

FSlateColor FPsDataBigIntegerStructCustomization::HandleTextBoxForegroundColor() const
{
	if (bInputValid)
	{
		static const FName InvertedForegroundName("InvertedForeground");
		return FEditorStyle::GetSlateColor(InvertedForegroundName);
	}

	return FLinearColor::Red;
}

FText FPsDataBigIntegerStructCustomization::HandleTextBoxText() const
{
	TArray<void*> RawData;
	PropertyHandle->AccessRawData(RawData);

	if (RawData.Num() != 1)
	{
		return LOCTEXT("MultipleValues", "Multiple Values");
	}

	if (RawData[0] == nullptr)
	{
		return FText::FromString("0");
	}

	return FText::FromString(static_cast<FPsDataBigInteger*>(RawData[0])->ToString());
}

void FPsDataBigIntegerStructCustomization::HandleTextBoxTextChanged(const FText& NewText)
{
	const auto BigIntegerOpt = PsDataTools::Numbers::ToNumber<FPsDataBigInteger>(PsDataTools::ToStringView(NewText.ToString()));
	bInputValid = BigIntegerOpt.IsSet();
}

void FPsDataBigIntegerStructCustomization::HandleTextBoxTextCommited(const FText& NewText, ETextCommit::Type CommitInfo)
{
	const auto BigIntegerOpt = PsDataTools::Numbers::ToNumber<FPsDataBigInteger>(PsDataTools::ToStringView(NewText.ToString()));
	if (BigIntegerOpt)
	{
		SetBigIntegerValue(*BigIntegerOpt);
	}
}

#undef LOCTEXT_NAMESPACE
