// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataNode_Property.h"

#include "UObject/Class.h"
#include "UObject/UObjectIterator.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintNodeSpawner.h"
#include "Styling/SlateIconFinder.h"
#include "BlueprintActionDatabaseRegistrar.h"

UPsDataNode_Property::UPsDataNode_Property(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUnsupportedType = false;
}

void UPsDataNode_Property::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
}

FLinearColor UPsDataNode_Property::GetNodeTitleColor() const
{
	if (HasError())
	{
		return FLinearColor::White;
	}

	return FLinearColor(0.f, 1.f, 0.f, 1.f);
}

FSlateIcon UPsDataNode_Property::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = Super::GetNodeTitleColor();
	return FSlateIcon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
}

bool UPsDataNode_Property::IsDeprecated() const
{
	const FDataFieldDescription* Field = GetProperty();
	if (Field)
	{
		return Field->Meta.bDeprecated;
	}
	return true;
}

void UPsDataNode_Property::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, bool bNodePure, UClass* TargetClass, const FDataFieldDescription& Field)
		{
			if (Field.Meta.bStrict && !bNodePure)
			{
				return nullptr;
			}
			
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);
			
			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, TargetClass](UEdGraphNode* EvaluatorNode, bool)
			{
				UPsDataNode_Property* Node = CastChecked<UPsDataNode_Property>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field.Name;
			});
			
			NodeSpawner->DefaultMenuSignature.Category = FText::FromString(FString::Printf(TEXT("PsData|%s"), *TargetClass->GetName()));
			
			return NodeSpawner;
		}
	};
	
	UClass* NodeClass = GetClass();
	for (UClass* Class : TObjectRange<UClass>())
	{
		for (auto& Pair : FDataReflection::GetFields(Class))
		{
			if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), IsNodePure(), Class, Pair.Value))
			{
				ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
			}
		}
	}
}

void UPsDataNode_Property::CreateExecPin()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
}

UEdGraphPin* UPsDataNode_Property::CreateTargetPin(EEdGraphPinDirection Dir, bool bShowName)
{
	if (TargetClass == nullptr)
	{
		return CreatePin(Dir, UEdGraphSchema_K2::PC_Wildcard, bShowName ? TEXT("Target") : TEXT(""));
	}
	else
	{
		return CreatePin(Dir, UEdGraphSchema_K2::PC_Object, NAME_None, TargetClass, bShowName ? TEXT("Target") : TEXT(""));
	}
}

UEdGraphPin* UPsDataNode_Property::CreatePropertyPin(EEdGraphPinDirection Dir, bool bShowName)
{
	if (!HasError())
	{
		const FDataFieldDescription* Field = GetProperty();
		
		UProperty* Property = nullptr;
		UFunction* Function = UPsData::StaticClass()->FindFunctionByName(FName(*FDataReflection::GenerateGetFunctionName(*Field)), EIncludeSuperFlag::IncludeSuper);
		for (TFieldIterator<UProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			if (PropIt->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				Property = *PropIt;
				break;
			}
		}
		
		if (Property)
		{
			UEdGraphPin* Pin = CreatePin(Dir, UEdGraphSchema_K2::PC_Wildcard, bShowName ? *PropertyName : TEXT(""));
			bUnsupportedType = !GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Property, Pin->PinType);
			
			if (Field->Type == EDataFieldType::DFT_Data && Field->Class)
			{
				Pin->PinType.PinSubCategoryObject = Field->Class;
			}
			
			return Pin;
		}
		else
		{
			bUnsupportedType = true;
		}
	}
	
	return CreatePin(Dir, UEdGraphSchema_K2::PC_Wildcard, bShowName ? *PropertyName : TEXT(""));
}

bool UPsDataNode_Property::HasError() const
{
	if (TargetClass == nullptr || bUnsupportedType || !FDataReflection::HasClass(TargetClass))
	{
		return true;
	}
	
	const FDataFieldDescription* Property = GetProperty();
	if (!Property)
	{
		return true;
	}
	
	if (Property->Type == EDataFieldType::DFT_Data && (Property->Class == nullptr || !FDataReflection::HasClass(Property->Class)))
	{
		return true;
	}
	
	return false;
}

const FDataFieldDescription* UPsDataNode_Property::GetProperty() const
{
	return FDataReflection::GetFields(TargetClass).Find(PropertyName);
}




