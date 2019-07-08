// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataNode_Variable.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2.h"
#include "Editor/UnrealEd/Public/Kismet2/CompilerResultsLog.h"
#include "Styling/SlateIconFinder.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_Variable"

UPsDataNode_Variable::UPsDataNode_Variable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetClass(nullptr)
{
}

void UPsDataNode_Variable::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
		{
			if (Pin->PinName.ToString() == TEXT("Target"))
			{
				UpdatePin(EPsDataVariablePinType::Target, Pin);
			}
			else if (Pin->PinName.ToString() == TEXT("Crc32"))
			{
				UpdatePin(EPsDataVariablePinType::PropertyHash, Pin);
			}
			else if (Pin->PinName.ToString() == TEXT("Value"))
			{
				UpdatePin(EPsDataVariablePinType::Value, Pin);
			}
			else
			{
				UpdatePin(EPsDataVariablePinType::Unknown, Pin);
			}
		}
		else if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			if (Pin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
			{
				UpdatePin(EPsDataVariablePinType::ReturnValue, Pin);
			}
			else
			{
				UpdatePin(EPsDataVariablePinType::Unknown, Pin);
			}
		}
	}
}

FText UPsDataNode_Variable::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FLinearColor UPsDataNode_Variable::GetNodeTitleColor() const
{
	auto Field = GetProperty();
	if (!Field.IsValid())
	{
		return FLinearColor::White;
	}

	UFunction* Function = Field->Context->GetUFunctions()->GetFunction;
	UProperty* Property = Function->GetReturnProperty();

	if (Property)
	{
		FEdGraphPinType PinType;
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		if (K2Schema->ConvertPropertyToPinType(Property, PinType))
		{
			return K2Schema->GetPinTypeColor(PinType);
		}
	}

	return FLinearColor::White;
}

FSlateIcon UPsDataNode_Variable::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = GetNodeTitleColor();

	auto Field = GetProperty();
	if (!Field.IsValid())
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.VariableIcon");
	}

	if (Field->Context->IsArray())
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.ArrayVariableIcon");
	}

	if (Field->Context->IsMap())
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.MapVariableKeyIcon");
	}

	return FSlateIcon("EditorStyle", "Kismet.AllClasses.VariableIcon");
}

bool UPsDataNode_Variable::IsDeprecated() const
{
	auto Field = GetProperty();
	if (Field.IsValid())
	{
		return Field->Meta.bDeprecated;
	}
	return Super::IsDeprecated();
}

void UPsDataNode_Variable::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	auto Field = GetProperty();
	if (!Field.IsValid())
	{
		MessageLog.Error(*LOCTEXT("UPsDataNode_Variable", "Node @@: property has been removed").ToString(), this);
	}
	else
	{
		if (PropertyCppType.IsEmpty())
		{
			MessageLog.Warning(*LOCTEXT("UPsDataNode_Variable", "Node @@ does not contain type information, please recreate the node").ToString(), this);
		}
		else if (PropertyCppType != Field->Context->GetCppType())
		{
			MessageLog.Error(*LOCTEXT("UPsDataNode_Variable", "Node @@: type of property has changed, please recreate the node").ToString(), this);
		}
	}
}

TSharedPtr<const FDataField> UPsDataNode_Variable::GetProperty() const
{
	auto Find = FDataReflection::GetFields(TargetClass).Find(PropertyName);
	if (Find)
	{
		return *Find;
	}

	return TSharedPtr<const FDataField>(nullptr);
}

void UPsDataNode_Variable::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin)
{
	auto Field = GetProperty();
	if (PinType == EPsDataVariablePinType::Target)
	{
		if (TargetClass == nullptr)
		{
			Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		}
		else
		{
			Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
			Pin->PinType.PinSubCategoryObject = TargetClass;
		}
	}
	else if (PinType == EPsDataVariablePinType::PropertyHash)
	{
		Pin->bHidden = true;
		if (Field.IsValid())
		{
			Pin->DefaultValue = FString::FromInt(Field->Hash);
			Pin->bDefaultValueIsReadOnly = true;
		}
	}
	else if (PinType == EPsDataVariablePinType::Value || PinType == EPsDataVariablePinType::ReturnValue)
	{
		if (PinType == EPsDataVariablePinType::Value)
		{
			Pin->PinFriendlyName = FText::FromString(PropertyName);
		}

		if (Field.IsValid() && Field->Context->GetUE4Type() != nullptr)
		{
			UField* UE4Type = Field->Context->GetUE4Type();

			if (UEnum* Enum = Cast<UEnum>(UE4Type))
			{
				if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte)
				{
					Pin->PinType.PinSubCategoryObject = Enum;
				}
			}
			else
			{
				if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject)
				{
					Pin->PinType.PinSubCategoryObject = UE4Type;
				}
				else if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object && Pin->PinType.PinSubCategoryObject == UPsData::StaticClass())
				{
					Pin->PinType.PinSubCategoryObject = UE4Type;
				}
				else if (Pin->PinType.ContainerType == EPinContainerType::Map && Pin->PinType.PinValueType.TerminalSubCategoryObject == UPsData::StaticClass())
				{
					Pin->PinType.PinValueType.TerminalSubCategoryObject = UE4Type;
				}
			}
		}
	}
}

void UPsDataNode_Variable::UpdateFunctionReference()
{
}

#undef LOCTEXT_NAMESPACE
