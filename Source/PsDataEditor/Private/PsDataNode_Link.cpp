// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

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
	const auto Link = GetLink();
	if (Link)
	{
		const FString FunctionName = (Link->bCollection ? TEXT("GetDataArrayByLinkHash") : TEXT("GetDataByLinkHash"));
		UFunction* Function = UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));

		FProperty* Property = nullptr;
		for (TFieldIterator<FProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			if (PropIt->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				Property = *PropIt;
				break;
			}
		}

		if (Property)
		{
			FEdGraphPinType PinType;
			const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
			if (K2Schema->ConvertPropertyToPinType(Property, PinType))
			{
				return K2Schema->GetPinTypeColor(PinType);
			}
		}
	}

	return FLinearColor::White;
}

FSlateIcon UPsDataNode_Link::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = GetNodeTitleColor();

	const auto Link = GetLink();
	if (Link && Link->bCollection)
	{
		return FSlateIcon("EditorStyle", "Kismet.AllClasses.ArrayVariableIcon");
	}

	return FSlateIcon("EditorStyle", "Kismet.AllClasses.VariableIcon");
}

FText UPsDataNode_Link::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_Link node"));
	}

	return FText::FromString(FString::Printf(TEXT("Get Link By %s"), *PropertyName));
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
		for (const auto Field : ClassFields->GetFieldsList())
		{
			const auto Link = ClassFields->GetLinkByHash(Field->Hash);
			if (Link)
			{
				if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, Field, Link))
				{
					ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
				}
			}
		}
	}
}

const FDataLink* UPsDataNode_Link::GetLink() const
{
	const auto Field = GetProperty();
	if (Field)
	{
		return PsDataTools::FDataReflection::GetFieldsByClass(TargetClass)->GetLinkByHash(Field->Hash);
	}

	return nullptr;
}

UClass* UPsDataNode_Link::GetReturnType(const FDataLink* Link) const
{
	FString ClassName;
	if (Link->ReturnType.EndsWith(TEXT("*"), ESearchCase::CaseSensitive))
	{
		ClassName = Link->ReturnType.Left(Link->ReturnType.Len() - 1);
	}
	else
	{
		ClassName = Link->ReturnType;
	}

	static TMap<FString, UClass*> Classes;
	if (Classes.Num() == 0)
	{
		for (TObjectIterator<UClass> It; It; ++It)
		{
			UClass* Class = *It;
			if (Class->IsChildOf(UPsData::StaticClass()))
			{
				Classes.Add(TEXT("U") + Class->GetName(), Class);
			}
		}
	}

	const auto Find = Classes.Find(ClassName);
	if (!Find)
	{
		return UPsData::StaticClass();
	}

	return *Find;
}

void UPsDataNode_Link::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin) const
{
	Super::UpdatePin(PinType, Pin);
	const auto Link = GetLink();

	if (PinType == EPsDataVariablePinType::PropertyOut || PinType == EPsDataVariablePinType::OldPropertyOut)
	{
		Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("Link By %s"), *PropertyName));
		if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
		{
			if (Link)
			{
				Pin->PinType.PinSubCategoryObject = GetReturnType(Link);
			}
		}
	}
	else if (PinType == EPsDataVariablePinType::Index)
	{
		Pin->bHidden = true;
		Pin->bNotConnectable = true;
		Pin->bDefaultValueIsReadOnly = true;

		if (Link)
		{
			Pin->DefaultValue = FString::FromInt(Link->Hash);
			Pin->AutogeneratedDefaultValue = Pin->DefaultValue;
		}
	}
}

UFunction* UPsDataNode_Link::GetFunction() const
{
	const auto Field = GetProperty();
	const auto Link = GetLink();
	if (!Field || !Link)
	{
		return nullptr;
	}

	const FString FunctionName = (Link->bCollection ? TEXT("GetDataArrayByLinkHash") : TEXT("GetDataByLinkHash"));
	UFunction* Function = UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));

	return Function;
}

#undef LOCTEXT_NAMESPACE
