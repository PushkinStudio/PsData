// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataCallFunctionHandler.h"

#include "PsDataDefines.h"

#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "FPsDataHandler_CallFunction"

FPsDataHandler_CallFunction::FPsDataHandler_CallFunction(FKismetCompilerContext& InCompilerContext)
	: FNodeHandlingFunctor(InCompilerContext)
{
}

void FPsDataHandler_CallFunction::RegisterNet(FKismetFunctionContext& Context, UEdGraphPin* Net)
{
	const auto Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));
	Context.NetMap.Add(Net, Term);
}

void FPsDataHandler_CallFunction::Compile(FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	check(Node != nullptr);

	if (!IsCalledFunctionPure(Node))
	{
		const auto ExecTriggeringPin = CompilerContext.GetSchema()->FindExecutionPin(*Node, EGPD_Input);
		if (!ExecTriggeringPin)
		{
			CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "NoValidExecutionPinForCallFunc_Error", "@@ must have a valid execution pin").ToString(), Node);
			return;
		}

		if (ExecTriggeringPin->LinkedTo.Num() == 0)
		{
			CompilerContext.MessageLog.Warning(*NSLOCTEXT("KismetCompiler", "NodeNeverExecuted_Warning", "@@ will never be executed").ToString(), Node);
			return;
		}
	}

	const auto SelfPin = CompilerContext.GetSchema()->FindSelfPin(*Node, EGPD_Input);
	if (SelfPin && SelfPin->LinkedTo.Num() == 0)
	{
		FEdGraphPinType SelfType;
		SelfType.PinCategory = UEdGraphSchema_K2::PC_Object;
		SelfType.PinSubCategory = UEdGraphSchema_K2::PSC_Self;

		if (!CompilerContext.GetSchema()->ArePinTypesCompatible(SelfType, SelfPin->PinType, Context.NewClass) && !SelfPin->DefaultObject)
		{
			CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "PinMustHaveConnectionPruned_Error", "Pin @@ must have a connection.  Self pins cannot be connected to nodes that are culled.").ToString(), SelfPin);
		}
	}

	CreateFunctionCallStatement(Context, Node, SelfPin);
}

bool FPsDataHandler_CallFunction::IsCalledFunctionPure(UEdGraphNode* Node) const
{
	if (const auto CallFunctionNode = Cast<UK2Node_CallFunction>(Node))
	{
		return CallFunctionNode->bIsPureFunc;
	}

	return false;
}

FName FPsDataHandler_CallFunction::GetFunctionNameFromNode(UEdGraphNode* Node) const
{
	if (const auto CallFuncNode = Cast<UK2Node_CallFunction>(Node))
	{
		return CallFuncNode->FunctionReference.GetMemberName();
	}

	CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "UnableResolveFunctionName_Error", "Unable to resolve function name for @@").ToString(), Node);
	return NAME_None;
}

UClass* FPsDataHandler_CallFunction::GetCallingContext(FKismetFunctionContext& Context, UEdGraphNode* Node) const
{
	if (const auto SelfPin = CompilerContext.GetSchema()->FindSelfPin(*Node, EGPD_Input))
	{
		return Cast<UClass>(Context.GetScopeFromPinType(SelfPin->PinType, Context.NewClass));
	}

	return Context.NewClass;
}

UFunction* FPsDataHandler_CallFunction::FindFunction(FKismetFunctionContext& Context, UEdGraphNode* Node) const
{
	if (const auto CallingContext = GetCallingContext(Context, Node))
	{
		return CallingContext->FindFunctionByName(GetFunctionNameFromNode(Node));
	}

	return nullptr;
}

void FPsDataHandler_CallFunction::CreateFunctionCallStatement(FKismetFunctionContext& Context, UEdGraphNode* Node, UEdGraphPin* SelfPin)
{
	const int32 NumErrorsAtStart = CompilerContext.MessageLog.NumErrors;
	if (const auto Function = FindFunction(Context, Node))
	{
		const auto Schema = CompilerContext.GetSchema();

		TArray<UEdGraphPin*> RemainingPins;
		RemainingPins.Append(Node->Pins);
		RemainingPins.RemoveAll([Schema](const UEdGraphPin* Pin) {
			return Pin->bOrphanedPin || Schema->IsMetaPin(*Pin);
		});

		FBPTerminal* LHSTerm = nullptr;
		TArray<FBPTerminal*> RHSTerms;
		for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
		{
			const auto Property = *It;
			bool bFoundParam = false;
			for (int32 i = 0; !bFoundParam && (i < RemainingPins.Num()); ++i)
			{
				const auto PinMatch = RemainingPins[i];
				if (Property->GetFName() == PinMatch->PinName)
				{
					//if (UK2Node_CallFunction::IsStructureWildcardProperty(Function, Property->GetFName()) ||
					//	FKismetCompilerUtilities::IsTypeCompatibleWithProperty(PinMatch, Property, CompilerContext.MessageLog, CompilerContext.GetSchema(), Context.NewClass))
					{
						const auto PinToTry = FEdGraphUtilities::GetNetFromPin(PinMatch);
						if (const auto Term = Context.NetMap.Find(PinToTry))
						{
							if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
							{
								LHSTerm = *Term;
							}
							else
							{
								RHSTerms.Add(*Term);
							}

							if (Property->HasAnyPropertyFlags(CPF_OutParm) && !((*Term)->IsTermWritable()))
							{
								if (Property->HasAnyPropertyFlags(CPF_ReferenceParm))
								{
									if (!Property->HasAnyPropertyFlags(CPF_ConstParm))
									{
										CompilerContext.MessageLog.Error(*LOCTEXT("PassReadOnlyReferenceParam_Error", "Cannot pass a read-only variable to a reference parameter @@").ToString(), PinMatch);
									}
								}
								else
								{
									CompilerContext.MessageLog.Error(*LOCTEXT("PassReadOnlyOutputParam_Error", "Cannot pass a read-only variable to a output parameter @@").ToString(), PinMatch);
								}
							}
						}
						else
						{
							CompilerContext.MessageLog.Error(*LOCTEXT("ResolveTermPassed_Error", "Failed to resolve term passed into @@").ToString(), PinMatch);
						}
					}

					bFoundParam = true;
					RemainingPins.RemoveAt(i);
				}
			}

			if (!bFoundParam)
			{
				CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("FindPinParameter_ErrorFmt", "Could not find a pin for the parameter {0} of {1} on @@"), FText::FromString(Property->GetName()), FText::FromString(Function->GetName())).ToString(), Node);
			}
		}

		for (const auto RemainingPin : RemainingPins)
		{
			CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("PinMismatchParameter_ErrorFmt", "Pin @@ named {0} doesn't match any parameters of function {1}"), FText::FromName(RemainingPin->PinName), FText::FromString(Function->GetName())).ToString(), RemainingPin);
		}

		if (NumErrorsAtStart == CompilerContext.MessageLog.NumErrors)
		{
			TArray<FBPTerminal*> ContextTerms;
			if (SelfPin)
			{
				if (SelfPin->LinkedTo.Num() > 0)
				{
					for (int32 i = 0; i < SelfPin->LinkedTo.Num(); i++)
					{
						const auto ContextTerm = Context.NetMap.Find(SelfPin->LinkedTo[i]);
						if (ensureMsgf(ContextTerm != nullptr, TEXT("'%s' is missing a target input - if this is a server build, the input may be a cosmetic only property which was discarded (if this is the case, and this is expecting component variable try resaving.)"), *Node->GetPathName()))
						{
							ContextTerms.Add(*ContextTerm);
						}
					}
				}
				else
				{
					const auto ContextTerm = Context.NetMap.Find(SelfPin);
					ContextTerms.Add(ContextTerm ? *ContextTerm : nullptr);
				}
			}

			const auto SrcEventNode = CompilerContext.CallsIntoUbergraph.Find(Node);
			for (const auto Target : ContextTerms)
			{
				FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
				Statement.FunctionToCall = Function;
				Statement.FunctionContext = Target;
				Statement.Type = KCST_CallFunction;
				Statement.bIsInterfaceContext = false;
				Statement.bIsParentContext = false;

				Statement.LHS = LHSTerm;
				Statement.RHS = RHSTerms;

				if (SrcEventNode)
				{
					UEdGraphPin* ExecOut = CompilerContext.GetSchema()->FindExecutionPin(**SrcEventNode, EGPD_Output);

					check(CompilerContext.UbergraphContext);
					CompilerContext.UbergraphContext->GotoFixupRequestMap.Add(&Statement, ExecOut);
					Statement.UbergraphCallIndex = 0;
				}

				if (Statement.Type == KCST_CallFunction && Function->HasAnyFunctionFlags(FUNC_Delegate))
				{
					CompilerContext.MessageLog.Error(*LOCTEXT("CallingDelegate_Error", "@@ is trying to call a delegate function - delegates cannot be called directly").ToString(), Node);
					Statement.Type = KCST_CallDelegate;
				}
			}

			if (!IsCalledFunctionPure(Node))
			{
				GenerateSimpleThenGoto(Context, *Node);
			}
		}
	}
	else
	{
		CompilerContext.MessageLog.Warning(*FText::Format(LOCTEXT("FindFunction_ErrorFmt", "Could not find the function '{0}' called from @@"), FText::FromString(GetFunctionNameFromNode(Node).ToString())).ToString(), Node);
	}
}

#undef LOCTEXT_NAMESPACE
