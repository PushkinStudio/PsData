// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "Customization/PsDataFixedPointStructCustomization.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "ScopedTransaction.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "FPsDataFixedPointStructCustomization"

void WriteFixedPointToProperty(TSharedPtr<IPropertyHandle> FixedPointPropertyHandle, const FPsDataFixedPoint& Value)
{
	FScopedTransaction Transaction(FText::Format(LOCTEXT("EditFixedPointPropertyTransaction", "Edit {0}"), FixedPointPropertyHandle->GetPropertyDisplayName()));

	TArray<void*> RawDatas;
	FixedPointPropertyHandle->AccessRawData(RawDatas);
	FixedPointPropertyHandle->NotifyPreChange();
	for (void* RawData : RawDatas)
	{
		auto Data = static_cast<FPsDataFixedPoint*>(RawData);
		*Data = Value;
	}
	FixedPointPropertyHandle->NotifyPostChange();
}

TSharedRef<IPropertyTypeCustomization> FPsDataFixedPointStructCustomization::MakeInstance()
{
	return MakeShareable(new FPsDataFixedPointStructCustomization);
}

void FPsDataFixedPointStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
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
						.ForegroundColor(this, &FPsDataFixedPointStructCustomization::HandleTextBoxForegroundColor)
						.OnTextChanged(this, &FPsDataFixedPointStructCustomization::HandleTextBoxTextChanged)
						.OnTextCommitted(this, &FPsDataFixedPointStructCustomization::HandleTextBoxTextCommited)
						.SelectAllTextOnCommit(true)
						.Text(this, &FPsDataFixedPointStructCustomization::HandleTextBoxText)
				]
		];
	// clang-format on
}

void FPsDataFixedPointStructCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FPsDataFixedPointStructCustomization::SetFixedPointValue(const FPsDataFixedPoint& Value)
{
	WriteFixedPointToProperty(PropertyHandle, Value);
}

FSlateColor FPsDataFixedPointStructCustomization::HandleTextBoxForegroundColor() const
{
	if (bInputValid)
	{
		static const FName InvertedForegroundName("InvertedForeground");
		return FEditorStyle::GetSlateColor(InvertedForegroundName);
	}

	return FLinearColor::Red;
}

FText FPsDataFixedPointStructCustomization::HandleTextBoxText() const
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

	return FText::FromString(static_cast<FPsDataFixedPoint*>(RawData[0])->ToString());
}

void FPsDataFixedPointStructCustomization::HandleTextBoxTextChanged(const FText& NewText)
{
	const auto FixedPointOpt = PsDataTools::Numbers::ToNumber<FPsDataFixedPoint>(PsDataTools::ToStringView(NewText.ToString()));
	bInputValid = FixedPointOpt.IsSet();
}

void FPsDataFixedPointStructCustomization::HandleTextBoxTextCommited(const FText& NewText, ETextCommit::Type CommitInfo)
{
	const auto FixedPointOpt = PsDataTools::Numbers::ToNumber<FPsDataFixedPoint>(PsDataTools::ToStringView(NewText.ToString()));
	if (FixedPointOpt)
	{
		SetFixedPointValue(*FixedPointOpt);
	}
}

#undef LOCTEXT_NAMESPACE
