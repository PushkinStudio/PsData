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
{
}

void UPsDataNode_Link::AllocateDefaultPins()
{
	auto Field = GetProperty();
	auto Link = GetLink();
	if (Field.IsValid() && Link.IsValid())
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
	auto Link = GetLink();
	if (Link.IsValid())
	{
		FString FunctionName = (Link->bCollection ? TEXT("GetDataArrayByLinkHash") : TEXT("GetDataByLinkHash"));
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
	}

	return FLinearColor::White;
}

FSlateIcon UPsDataNode_Link::GetIconAndTint(FLinearColor& ColorOut) const
{
	ColorOut = GetNodeTitleColor();

	auto Link = GetLink();
	if (Link.IsValid() && Link->bCollection)
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
		static UBlueprintNodeSpawner* MakeAction(TSubclassOf<UEdGraphNode> NodeClass, UClass* TargetClass, const FDataField& Field, const FDataLink& Link)
		{
			UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(NodeClass);
			check(NodeSpawner != nullptr);

			NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateLambda([Field, Link, TargetClass](UEdGraphNode* EvaluatorNode, bool) {
				UPsDataNode_Link* Node = CastChecked<UPsDataNode_Link>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field.Name;
				Node->PropertyCppType = Field.Context->GetCppType();
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
			TSharedPtr<const FDataLink> Link = FDataReflection::GetLinkByHash(Class, Pair.Value->Hash);
			if (Link.IsValid())
			{
				if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), Class, *Pair.Value, *Link))
				{
					ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
				}
			}
		}
	}
}

TSharedPtr<const FDataLink> UPsDataNode_Link::GetLink() const
{
	auto Field = GetProperty();
	if (Field.IsValid())
	{
		return FDataReflection::GetLinkByHash(TargetClass, Field->Hash);
	}
	return TSharedPtr<const FDataLink>(nullptr);
}

UClass* UPsDataNode_Link::GetReturnType(TSharedPtr<const FDataLink> Link)
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

	auto Find = Classes.Find(ClassName);
	if (!Find)
	{
		return UPsData::StaticClass();
	}

	return *Find;
}

void UPsDataNode_Link::UpdatePin(EPsDataVariablePinType PinType, UEdGraphPin* Pin)
{
	Super::UpdatePin(PinType, Pin);
	auto Link = GetLink();

	if (PinType == EPsDataVariablePinType::ReturnValue)
	{
		Pin->PinFriendlyName = FText::FromString(FString::Printf(TEXT("Link By %s"), *PropertyName));
		if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
		{
			if (Link.IsValid())
			{
				Pin->PinType.PinSubCategoryObject = GetReturnType(Link);
			}
		}
	}
	else if (PinType == EPsDataVariablePinType::PropertyHash)
	{
		if (Link.IsValid())
		{
			Pin->bHidden = true;
			Pin->DefaultValue = FString::FromInt(Link->Hash);
		}
	}
}

void UPsDataNode_Link::UpdateFunctionReference()
{
	auto Field = GetProperty();
	auto Link = GetLink();
	if (!Field.IsValid() || !Link.IsValid())
	{
		return;
	}

	FString FunctionName = (Link->bCollection ? TEXT("GetDataArrayByLinkHash") : TEXT("GetDataByLinkHash"));
	UFunction* Function = UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(*FunctionName));
	if (Function)
	{
		SetFromFunction(Function);
	}
}

#undef LOCTEXT_NAMESPACE
