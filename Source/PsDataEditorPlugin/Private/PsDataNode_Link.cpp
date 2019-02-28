// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

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
	, Path()
	, ReturnType(nullptr)
	, bCollection(false)
{
}

void UPsDataNode_Link::AllocateDefaultPins()
{
	auto Field = GetProperty();
	if (Field.IsValid())
	{
		SetLinkPath(Field->Meta.LinkPath);
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
	FString FunctionName = (bCollection ? TEXT("GetDataArrayByPath") : TEXT("GetDataByPath"));
	UFunction* Function = UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));

	UProperty* Property = nullptr;
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
		FEdGraphPinType PinType;
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		if (K2Schema->ConvertPropertyToPinType(Property, PinType))
		{
			return K2Schema->GetPinTypeColor(PinType);
		}
	}

	return FLinearColor::White;
}

FSlateIcon UPsDataNode_Link::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = GetNodeTitleColor();
	if (bCollection)
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

	switch (TitleType)
	{
	case ENodeTitleType::MenuTitle:
		return FText::FromString(FString::Printf(TEXT("Get Link By %s"), *PropertyName));
	case ENodeTitleType::FullTitle:
		return FText::FromString(FString::Printf(TEXT("Get Link By %s::%s"), *TargetClass->GetName(), *PropertyName));
	default:
		return FText::FromString(TEXT("Get Link"));
	}
}

void UPsDataNode_Link::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	struct GetMenuActions_Utils
	{
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass, const FDataField& Field)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);

			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, TargetClass](UEdGraphNode* EvaluatorNode, bool) {
				UPsDataNode_Link* Node = CastChecked<UPsDataNode_Link>(EvaluatorNode);
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
			if (Pair.Value->Meta.bLink)
			{
				if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, *Pair.Value.Get()))
				{
					ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
				}
			}
		}
	}
}

void UPsDataNode_Link::SetLinkPath(const FString& LinkPath)
{
	TArray<FString> Values;
	LinkPath.ParseIntoArray(Values, TEXT("::"));

	if (Values.Num() != 3)
	{
		return;
	}

	bCollection = (Values[0] == TEXT("TArray<FString>"));
	Path = Values[1];

	FString ClassName;
	if (Values[2].EndsWith(TEXT("*"), ESearchCase::CaseSensitive))
	{
		ClassName = Values[2].Left(Values[2].Len() - 1);
	}
	else
	{
		ClassName = Values[2];
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

	auto Find = Classes.Find(ClassName);
	if (!Find)
	{
		return;
	}

	ReturnType = *Find;
}

void UPsDataNode_Link::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin)
{
	Super::UpdatePin(PinType, Pin);
	if (PinType == EPsDataVariablePinType::ReturnValue)
	{
		Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("Link By %s"), *PropertyName));
		if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
		{
			Pin->PinType.PinSubCategoryObject = ReturnType;
		}
	}
	else if (PinType == EPsDataVariablePinType::Unknown)
	{
		if (Pin->PinName.ToString() == TEXT("PropertyName"))
		{
			Pin->bHidden = true;
			Pin->DefaultValue = PropertyName;
		}
		else if (Pin->PinName.ToString() == TEXT("Path"))
		{
			Pin->bHidden = true;
			Pin->DefaultValue = Path;
		}
	}
}

void UPsDataNode_Link::UpdateFunctionReference()
{
	auto Field = GetProperty();
	if (!Field.IsValid())
	{
		return;
	}

	FString FunctionName = (bCollection ? TEXT("GetDataArrayByPath") : TEXT("GetDataByPath"));
	UFunction* Function = UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));
	if (Function)
	{
		SetFromFunction(Function);
	}
}

#undef LOCTEXT_NAMESPACE
