// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsDataEditorPlugin.h"
#include "PsDataNode_Link.h"
#include "PsDataAPI.h"

#include "KismetCompiler.h"
#include "EditorCategoryUtils.h"
#include "EdGraphUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "PsDataNode_GetProperty"

class FKCHandler_Link : public FNodeHandlingFunctor
{
public:
	FKCHandler_Link(FKismetCompilerContext& InCompilerContext)
		: FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_Link* PropNode = CastChecked<UPsDataNode_Link>(Node);
		if (PropNode->HasError())
		{
			Context.MessageLog.Error(TEXT("Bad Node"));
			return;
		}

		UEdGraphPin* OutputPin = PropNode->GetOutputPin();
		if (OutputPin->LinkedTo.Num() == 0)
		{
			return;
		}

		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputPin());
		FBPTerminal** TargetTerm = Context.NetMap.Find(InputTargetNet);
		if (TargetTerm == nullptr)
		{
			return;
		}

		FString FunctionName;
		if (PropNode->bCollection)
		{
			FunctionName = TEXT("GetDataArrayProperty_Link");
		}
		else
		{
			FunctionName = TEXT("GetDataProperty_Link");
		}
		
		UFunction* Function = UPsData::StaticClass()->FindFunctionByName(FName(*FunctionName));
		if (Function == nullptr)
		{
			Context.MessageLog.Error(*FString::Printf(TEXT("Function \"%s\" not found"), *FunctionName));
			return;
		}
		
		// Create get statment
		FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
		Statement.Type = KCST_CallFunction;
		Statement.FunctionToCall = Function;
		Statement.FunctionContext = *TargetTerm;
		Statement.bIsParentContext = false;
		
		{
			// First argument
			FBPTerminal* ArgFieldNameTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
			ArgFieldNameTerm->Type.PinCategory = UEdGraphSchema_K2::PC_String;
			ArgFieldNameTerm->Name = PropNode->Path;
			ArgFieldNameTerm->TextLiteral = FText::FromString(PropNode->Path);
			Statement.RHS.Add(ArgFieldNameTerm);
		}
		
		{
			// Second argument
			FBPTerminal* ArgFieldNameTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
			ArgFieldNameTerm->Type.PinCategory = UEdGraphSchema_K2::PC_String;
			ArgFieldNameTerm->Name = PropNode->PropertyName;
			ArgFieldNameTerm->TextLiteral = FText::FromString(PropNode->PropertyName);
			Statement.RHS.Add(ArgFieldNameTerm);
		}
		
		// Result
		Statement.LHS = *Context.NetMap.Find(OutputPin);
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		UPsDataNode_Link* PropNode = CastChecked<UPsDataNode_Link>(Node);
		FNodeHandlingFunctor::RegisterNets(Context, PropNode);

		UEdGraphPin* InputTargetNet = FEdGraphUtilities::GetNetFromPin(PropNode->GetInputPin());
		if (!Context.NetMap.Contains(InputTargetNet))
		{
			Context.NetMap.Add(InputTargetNet, Context.CreateLocalTerminalFromPinAutoChooseScope(InputTargetNet, Context.NetNameMap->MakeValidName(InputTargetNet)));
		}

		UEdGraphPin* OutputPin = PropNode->GetOutputPin();
		FBPTerminal* OutputTerm = Context.CreateLocalTerminalFromPinAutoChooseScope(OutputPin, Context.NetNameMap->MakeValidName(OutputPin));
		Context.NetMap.Add(OutputPin, OutputTerm);
	}

};

UPsDataNode_Link::UPsDataNode_Link(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, Path()
, ReturnType(nullptr)
, bCollection(false)
{
	
}

void UPsDataNode_Link::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreateTargetPin(EGPD_Input);
	CreatePropertyPin(EGPD_Output);
}

FText UPsDataNode_Link::GetMenuCategory() const
{
	return FText::FromString(TEXT("PsData"));
}

FText UPsDataNode_Link::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TargetClass == nullptr)
	{
		return FText::FromString(TEXT("Bad UPsDataNode_Link node"));
	}

	switch (TitleType) {
	case ENodeTitleType::MenuTitle:
		return FText::FromString(FString::Printf(TEXT("Get Link By %s"), *PropertyName));
	case ENodeTitleType::FullTitle:
		return FText::FromString(FString::Printf(TEXT("Get Link By %s::%s"), *TargetClass->GetName(), *PropertyName));
	default:
		return FText::FromString(TEXT("Get Link"));
	}
}

FNodeHandlingFunctor* UPsDataNode_Link::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_Link(CompilerContext);
}

void UPsDataNode_Link::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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
				UPsDataNode_Link* Node = CastChecked<UPsDataNode_Link>(EvaluatorNode);
				Node->TargetClass = TargetClass;
				Node->PropertyName = Field.Name;
				Node->SetLinkPath(Field.Meta.LinkPath);
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
			if (Pair.Value.Meta.bLink)
			{
				if (UBlueprintNodeSpawner* NodeSpawner = GetMenuActions_Utils::MakeAction(GetClass(), IsNodePure(), Class, Pair.Value))
				{
					ActionRegistrar.AddBlueprintAction(Class, NodeSpawner);
				}
			}
		}
	}
}

UEdGraphPin* UPsDataNode_Link::CreatePropertyPin(EEdGraphPinDirection Dir, bool bShowName)
{
	if (!HasError())
	{
		UEdGraphPin* Pin = CreatePin(Dir, UEdGraphSchema_K2::PC_Object, bShowName ? *(TEXT("Link By ") + PropertyName) : TEXT(""));
		Pin->PinType.PinSubCategoryObject = ReturnType;
		if (bCollection)
		{
			Pin->PinType.ContainerType = EPinContainerType::Array;
		}
		else
		{
			Pin->PinType.ContainerType = EPinContainerType::None;
		}

		return Pin;
	}

	return CreatePin(Dir, UEdGraphSchema_K2::PC_Wildcard, bShowName ? *PropertyName : TEXT(""));
}

UEdGraphPin* UPsDataNode_Link::GetInputPin()
{
	UEdGraphPin* InputPin = NULL;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Input)
		{
			return Pin;
		}
	}

	return nullptr;
}

UEdGraphPin* UPsDataNode_Link::GetOutputPin()
{
	UEdGraphPin* InputPin = NULL;
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin && Pin->Direction == EGPD_Output)
		{
			return Pin;
		}
	}

	return nullptr;
}

void UPsDataNode_Link::SetLinkPath(const FString& LinkPath)
{
	TArray<FString> Values;
	LinkPath.ParseIntoArray(Values, TEXT("::"));
	
	if (Values.Num() != 3)
	{
		bUnsupportedType = true;
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
		bUnsupportedType = true;
		return;
	}

	ReturnType = *Find;
}

#undef LOCTEXT_NAMESPACE
