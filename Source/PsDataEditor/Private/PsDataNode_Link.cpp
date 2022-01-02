// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataNode_Link.h"

#include "PsDataAPI.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "EdGraphUtilities.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "PsDataNode_Link"

UPsDataNode_Link::UPsDataNode_Link(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPsDataNode_Link::AllocateDefaultPins()
{
	const auto Field = GetProperty();
	const auto Link = GetLink();
	if (Field && Link)
	{
		UpdateFunctionReference();
	}
	Super::AllocateDefaultPins();
}

FText UPsDataNode_Link::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FLinearColor UPsDataNode_Link::GetNodeTitleColor() const
{
	const auto Field = GetProperty();
	if (!Field)
	{
		return FLinearColor::White;
	}

	UFunction* Function = Field->Context->GetLinkUFunctions().ResolveFunction();
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

FSlateIcon UPsDataNode_Link::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = GetNodeTitleColor();

	const auto Link = GetLink();
	if (!Link)
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.VariableIcon");
	}

	if (Link->ReturnContext->IsArray())
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.ArrayVariableIcon");
	}

	if (Link->ReturnContext->IsMap())
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.MapVariableKeyIcon");
	}

	return FSlateIcon("EditorStyle", "Kismet.AllClasses.VariableIcon");
}

FText UPsDataNode_Link::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_Link node"));
	}

	const auto Link = GetLink();
	if (Link && Link->bAbstract)
	{
		return FText::FromString(FString::Printf(TEXT("Get Link By Abstract %s"), *PropertyName));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("Get Link By %s"), *PropertyName));
	}
}

void UPsDataNode_Link::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass, const FDataField* Field, const FDataLink* Link)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);

			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, Link, TargetClass](UEdGraphNode* EvaluatorNode, bool) {
				UPsDataNode_Link* Node = CastChecked<UPsDataNode_Link>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field->Name;
			});

			NodeSpawner->DefaultMenuSignature.Category = FText::FromString(FString::Printf(TEXT("PsData|%s"), *TargetClass->GetName()));

			return NodeSpawner;
		}
	};

	UClass* NodeClass = GetClass();
	for (UClass* Class : TObjectRange<UClass>())
	{
		const auto ClassFields = PsDataTools::FDataReflection::GetFieldsByClass(Class);
		for (const auto Link : ClassFields->GetLinksList())
		{
			if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, Link->Field, Link))
			{
				ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
			}
		}
	}
}

const FDataLink* UPsDataNode_Link::GetLink() const
{
	if (const auto Field = GetProperty())
	{
		const auto Link = PsDataTools::FDataReflection::GetFieldsByClass(TargetClass)->GetLinkByHash(Field->Hash);
		if (Link)
		{
			return Link;
		}

		const auto AbstractLink = PsDataTools::FDataReflection::GetFieldsByClass(TargetClass)->GetLinkByHash(HashCombine(Field->Hash, PSDATA_ABSTRACT_LINK_SALT));
		if (AbstractLink)
		{
			return AbstractLink;
		}
	}

	return nullptr;
}

void UPsDataNode_Link::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin) const
{
	const auto Link = GetLink();
	if (Link)
	{
		UpdatePinByContext(PinType, Pin, Link->Index, Link->ReturnContext);
	}
}

void UPsDataNode_Link::UpdatePinByContext(EPsDataVariablePinType PinType, UEdGraphPin* Pin, int32 Index, FAbstractDataTypeContext* Context) const
{
	Super::UpdatePinByContext(PinType, Pin, Index, Context);
	if (PinType == EPsDataVariablePinType::PropertyOut || PinType == EPsDataVariablePinType::OldPropertyOut)
	{
		const auto Link = GetLink();
		if (Link && Link->bAbstract)
		{
			Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("Link By Abstract %s"), *PropertyName));
		}
		else
		{
			Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("Link By %s"), *PropertyName));
		}
	}
}

UFunction* UPsDataNode_Link::GetFunction() const
{
	const auto Link = GetLink();
	if (!Link)
	{
		return nullptr;
	}

	return Link->ReturnContext->GetLinkUFunctions().ResolveFunction();
}

#undef LOCTEXT_NAMESPACE
