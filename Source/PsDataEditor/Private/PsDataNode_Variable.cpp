// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsDataNode_Variable.h"

#include "PsDataCallFunctionHandler.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2.h"
#include "Editor/UnrealEd/Public/Kismet2/CompilerResultsLog.h"
#include "Styling/SlateIconFinder.h"
#include "UObject/CoreRedirects.h"

#define LOCTEXT_NAMESPACE "UPsDataNode_Variable"

const FName UPsDataNode_Variable::MD_PsDataTarget = "PsDataTarget";
const FName UPsDataNode_Variable::MD_PsDataIndex = "PsDataIndex";
const FName UPsDataNode_Variable::MD_PsDataOut = "PsDataOut";
const FName UPsDataNode_Variable::MD_PsDataIn = "PsDataIn";

const FName UPsDataNode_Variable::Default_TargetParam = "Target";
const FName UPsDataNode_Variable::Default_IndexParam = "Index";
const FName UPsDataNode_Variable::Default_OutParam = "Out";
const FName UPsDataNode_Variable::Default_InParam = "Value";

UPsDataNode_Variable::UPsDataNode_Variable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetClass(nullptr)
{
}

void UPsDataNode_Variable::AllocateDefaultPins()
{
	const bool bInvalidFunction = (GetTargetFunction() == nullptr);
	if (bInvalidFunction)
	{
		UpdateFunctionReference();
		// Modify();
	}

	Super::AllocateDefaultPins();

	const auto Function = GetTargetFunction();
	if (Function)
	{
		const auto TargetParam = Function->HasMetaData(MD_PsDataTarget) ? FName(*Function->GetMetaData(MD_PsDataTarget)) : Default_TargetParam;
		const auto IndexParam = Function->HasMetaData(MD_PsDataIndex) ? FName(*Function->GetMetaData(MD_PsDataIndex)) : Default_IndexParam;
		const auto OutParam = Function->HasMetaData(MD_PsDataOut) ? FName(*Function->GetMetaData(MD_PsDataOut)) : Default_OutParam;
		const auto InParam = Function->HasMetaData(MD_PsDataIn) ? FName(*Function->GetMetaData(MD_PsDataIn)) : Default_InParam;

		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				if (Pin->PinName == TargetParam)
				{
					UpdatePin(EPsDataVariablePinType::Target, Pin);
				}
				else if (Pin->PinName == IndexParam)
				{
					UpdatePin(EPsDataVariablePinType::Index, Pin);
				}
				else if (Pin->PinName == InParam)
				{
					UpdatePin(EPsDataVariablePinType::PropertyIn, Pin);
				}
				else
				{
					UpdatePin(EPsDataVariablePinType::Unknown, Pin);
				}
			}
			else if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
			{
				if (Pin->PinName == OutParam)
				{
					UpdatePin(EPsDataVariablePinType::PropertyOut, Pin);
				}
				else if (Pin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
				{
					UpdatePin(EPsDataVariablePinType::OldPropertyOut, Pin);
				}
				else
				{
					UpdatePin(EPsDataVariablePinType::Unknown, Pin);
				}
			}
		}
	}
}

UK2Node::ERedirectType UPsDataNode_Variable::DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	const ERedirectType RedirectType = Super::DoPinsMatchForReconstruction(NewPin, NewPinIndex, OldPin, OldPinIndex);
	if (RedirectType == ERedirectType_None && NewPin->Direction == OldPin->Direction)
	{
		if (OldPin->PinName == UEdGraphSchema_K2::PN_ReturnValue)
		{
			const auto Function = GetTargetFunction();
			if (Function)
			{
				const auto OutParam = Function->HasMetaData(MD_PsDataOut) ? FName(*Function->GetMetaData(MD_PsDataOut)) : Default_OutParam;
				if (NewPin->PinName == OutParam)
				{
					return ERedirectType_Name;
				}
			}
		}
	}
	return RedirectType;
}

FText UPsDataNode_Variable::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FLinearColor UPsDataNode_Variable::GetNodeTitleColor() const
{
	const auto Field = GetProperty();
	if (!Field)
	{
		return FLinearColor::White;
	}

	UFunction* Function = Field->Context->GetUFunctions().ResolveGetFunction();
	FProperty* OutProperty = Function->GetReturnProperty();

	if (!OutProperty)
	{
		const auto OutParam = Function->HasMetaData(MD_PsDataOut) ? FName(*Function->GetMetaData(MD_PsDataOut)) : Default_OutParam;
		OutProperty = Function->FindPropertyByName(OutParam);
	}

	if (OutProperty)
	{
		FEdGraphPinType PinType;
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		if (K2Schema->ConvertPropertyToPinType(OutProperty, PinType))
		{
			return K2Schema->GetPinTypeColor(PinType);
		}
	}

	return FLinearColor::White;
}

FSlateIcon UPsDataNode_Variable::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = GetNodeTitleColor();

	const auto Field = GetProperty();
	if (!Field)
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
	const auto Field = GetProperty();
	if (Field)
	{
		return Field->Meta.bDeprecated;
	}
	return Super::IsDeprecated();
}

bool ObjectCompare(UObject* ObjectA, UObject* ObjectB)
{
	if (ObjectA != ObjectB)
	{
		if (ObjectA && ObjectB)
		{
			const auto StructA = Cast<UStruct>(ObjectA);
			const auto StructB = Cast<UStruct>(ObjectB);

			if (StructA && StructB)
			{
				return StructA->IsChildOf(StructB);
			}
			else
			{
				UEnum* EnumA = Cast<UEnum>(ObjectA);
				UEnum* EnumB = Cast<UEnum>(ObjectB);

				if (EnumA && EnumB)
				{
					return EnumA == EnumB;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			UEnum* EnumA = Cast<UEnum>(ObjectA);
			UEnum* EnumB = Cast<UEnum>(ObjectB);

			if ((EnumA != nullptr && EnumB == nullptr) || (EnumA == nullptr && EnumB != nullptr))
			{
				return true;
			}

			return false;
		}
	}

	return true;
}

bool PinTypeCompare(const FEdGraphPinType& PinTypeA, const FEdGraphPinType& PinTypeB)
{
	if (PinTypeA.PinCategory != PinTypeB.PinCategory ||
		PinTypeA.PinSubCategory != PinTypeB.PinSubCategory)
	{
		return false;
	}

	if (PinTypeA.PinValueType.TerminalCategory != PinTypeB.PinValueType.TerminalCategory ||
		PinTypeA.PinValueType.TerminalSubCategory != PinTypeB.PinValueType.TerminalSubCategory)
	{
		return false;
	}

	if (!ObjectCompare(PinTypeA.PinSubCategoryObject.Get(), PinTypeB.PinSubCategoryObject.Get()))
	{
		return false;
	}

	if (!ObjectCompare(PinTypeA.PinValueType.TerminalSubCategoryObject.Get(), PinTypeB.PinValueType.TerminalSubCategoryObject.Get()))
	{
		return false;
	}

	return true;
}

void UPsDataNode_Variable::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	const auto Field = GetProperty();
	if (!Field)
	{
		MessageLog.Error(*LOCTEXT("UPsDataNode_Variable", "Node @@: property has been removed").ToString(), this);
		return;
	}

	const auto Function = GetFunction();
	if (Function)
	{
		FMemberReference NodeFunctionReference = FunctionReference;
		FMemberReference PropertyFunctionReference;
		PropertyFunctionReference.SetFromField<UFunction>(Function, GetBlueprintClassFromNode());
		if (!NodeFunctionReference.IsSameReference(PropertyFunctionReference))
		{
			MessageLog.Error(*LOCTEXT("UPsDataNode_Variable", "Node @@: type of property has changed").ToString(), this);
		}
	}
	else
	{
		MessageLog.Error(*LOCTEXT("UPsDataNode_Variable", "Node @@: illegal function").ToString(), this);
	}

	for (auto Pin : Pins)
	{
		for (auto Link : Pin->LinkedTo)
		{
			if (Pin->Direction == EEdGraphPinDirection::EGPD_Input)
			{
				if (!PinTypeCompare(Link->PinType, Pin->PinType))
				{
					MessageLog.Error(*FText::Format(LOCTEXT("UPsDataNode_Variable", "Node @@: illegal connection to pin \"{0}\""), Pin->GetDisplayName()).ToString(), this);
				}
			}
			else
			{
				if (!PinTypeCompare(Pin->PinType, Link->PinType))
				{
					MessageLog.Error(*FText::Format(LOCTEXT("UPsDataNode_Variable", "Node @@: illegal connection from pin \"{0}\""), Pin->GetDisplayName()).ToString(), this);
				}
			}
		}
	}
}

FNodeHandlingFunctor* UPsDataNode_Variable::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FPsDataHandler_CallFunction(CompilerContext);
}

const FDataField* UPsDataNode_Variable::GetProperty() const
{
	const auto Field = PsDataTools::FDataReflection::GetFieldsByClass(TargetClass)->GetFieldByName(PropertyName);
	if (Field)
	{
		return Field;
	}

	return nullptr;
}

void UPsDataNode_Variable::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin) const
{
	const auto Field = GetProperty();
	if (Field)
	{
		UpdatePinByContext(PinType, Pin, Field->Index, Field->Context);
	}
}

void UPsDataNode_Variable::UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const
{
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
	else if (PinType == EPsDataVariablePinType::Index)
	{
		Pin->bHidden = true;
		Pin->bNotConnectable = true;
		Pin->bDefaultValueIsReadOnly = true;

		Pin->DefaultValue = FString::FromInt(Index);
		Pin->AutogeneratedDefaultValue = Pin->DefaultValue;
	}
	else if (PinType == EPsDataVariablePinType::PropertyIn || PinType == EPsDataVariablePinType::PropertyOut || PinType == EPsDataVariablePinType::OldPropertyOut)
	{
		if (PinType == EPsDataVariablePinType::PropertyIn)
		{
			Pin->PinFriendlyName = FText::FromString(PropertyName);
		}

		if (Context->GetUE4Type() != nullptr)
		{
			UField* UE4Type = Context->GetUE4Type();
			UEnum* Enum = Cast<UEnum>(UE4Type);

			if (Enum)
			{
				Pin->PinType.PinCategory = UEdGraphSchema_K2::PC_Byte;
				Pin->PinType.PinSubCategoryObject = Enum;
			}
			else
			{
				if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_SoftObject ||
					Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_SoftClass)
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

UFunction* UPsDataNode_Variable::GetFunction() const
{
	return nullptr;
}

void UPsDataNode_Variable::UpdateFunctionReference()
{
	const auto Function = GetFunction();
	if (Function)
	{
		SetFromFunction(Function);
	}
}

#undef LOCTEXT_NAMESPACE
