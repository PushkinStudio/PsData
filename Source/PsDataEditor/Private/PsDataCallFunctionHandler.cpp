// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsDataCallFunctionHandler.h"

#include "BlueprintCompilationManager.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_CallParentFunction.h"
#include "K2Node_Event.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_Self.h"
#include "K2Node_VariableGet.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/MetaData.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphUtilities.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Self.h"
#include "KismetCompiledFunctionContext.h"
#include "KismetCompiler.h"
#include "Net/Core/PushModel/PushModelMacros.h"
#include "Net/NetPushModelHelpers.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

#if WITH_PUSH_MODEL
#include "Net/NetPushModelHelpers.h"
#endif

#define LOCTEXT_NAMESPACE "PsDataHandler_CallFunction"

//////////////////////////////////////////////////////////////////////////
// FImportTextErrorContext
// Based on original FImportTextErrorContext
// Original code Copyright Epic Games, Inc.

class FImportTextErrorContext : public FOutputDevice
{
protected:
	FCompilerResultsLog& MessageLog;
	UObject* TargetObject;

public:
	int32 NumErrors;

	FImportTextErrorContext(FCompilerResultsLog& InMessageLog, UObject* InTargetObject)
		: FOutputDevice()
		, MessageLog(InMessageLog)
		, TargetObject(InTargetObject)
		, NumErrors(0)
	{
	}

	virtual void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category) override
	{
		if (TargetObject == nullptr)
		{
			MessageLog.Error(V);
		}
		else
		{
			const FString ErrorString = FString::Printf(TEXT("Invalid default on node @@: %s"), V);
			MessageLog.Error(*ErrorString, TargetObject);
		}
		NumErrors++;
	}
};

//////////////////////////////////////////////////////////////////////////
// PsDataHandler_CallFunction
// Based on vanilla UE CallFunctionHandler with type checking modifications
// Original code Copyright Epic Games, Inc.

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4750)
#endif

void FPsDataHandler_CallFunction::CreateFunctionCallStatement(FKismetFunctionContext& Context, UEdGraphNode* Node, UEdGraphPin* SelfPin)
{
	int32 NumErrorsAtStart = CompilerContext.MessageLog.NumErrors;

	if (UFunction* Function = FindFunction(Context, Node))
	{
		CheckIfFunctionIsCallable(Function, Context, Node);

		TArray<UEdGraphPin*> RemainingPins;
		RemainingPins.Append(Node->Pins);

		const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();

		RemainingPins.RemoveAll([Schema](UEdGraphPin* Pin) { return (Pin->bOrphanedPin || Schema->IsMetaPin(*Pin)); });

		const bool bIsLatent = Function->HasMetaData(FBlueprintMetadata::MD_Latent);
		if (bIsLatent && (CompilerContext.UbergraphContext != &Context))
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ContainsLatentCall_Error", "@@ contains a latent call, which cannot exist outside of the event graph").ToString(), Node);
		}

		UEdGraphPin* LatentInfoPin = nullptr;

		if (TMap<FName, FString>* MetaData = UMetaData::GetMapForObject(Function))
		{
			for (TMap<FName, FString>::TConstIterator It(*MetaData); It; ++It)
			{
				const FName& Key = It.Key();

				if (Key == TEXT("LatentInfo"))
				{
					UEdGraphPin* Pin = Node->FindPin(It.Value());
					if (Pin && (Pin->Direction == EGPD_Input) && (Pin->LinkedTo.Num() == 0))
					{
						LatentInfoPin = Pin;

						UEdGraphPin* PinToTry = FEdGraphUtilities::GetNetFromPin(Pin);
						if (FBPTerminal** Term = Context.NetMap.Find(PinToTry))
						{
							check((*Term)->bIsLiteral);

							int32 LatentUUID = CompilerContext.MessageLog.CalculateStableIdentifierForLatentActionManager(LatentInfoPin->GetOwningNode());

							const FString ExecutionFunctionName = UEdGraphSchema_K2::FN_ExecuteUbergraphBase.ToString() + TEXT("_") + Context.Blueprint->GetName();
							(*Term)->Name = FString::Printf(TEXT("(Linkage=%s,UUID=%s,ExecutionFunction=%s,CallbackTarget=None)"), *FString::FromInt(INDEX_NONE), *FString::FromInt(LatentUUID), *ExecutionFunctionName);

							UEdGraphNode* TrueSourceNode = Cast<UEdGraphNode>(Context.MessageLog.FindSourceObject(Node));
							Context.NewClass->GetDebugData().RegisterUUIDAssociation(TrueSourceNode, LatentUUID);
						}
					}
					else
					{
						CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("FindPinFromLinkage_ErrorFmt", "Function {0} (called from @@) was specified with LatentInfo metadata but does not have a pin named {1}"),
															 FText::FromString(Function->GetName()), FText::FromString(It.Value()))
															  .ToString(),
							Node);
					}
				}
			}
		}

		FBPTerminal* LHSTerm = nullptr;
		TArray<FBPTerminal*> RHSTerms;
		UEdGraphPin* ThenExecPin = nullptr;
		UEdGraphNode* LatentTargetNode = nullptr;
		int32 LatentTargetParamIndex = INDEX_NONE;

		UScriptStruct* VectorStruct = TBaseStructure<FVector>::Get();
		UScriptStruct* RotatorStruct = TBaseStructure<FRotator>::Get();
		UScriptStruct* TransformStruct = TBaseStructure<FTransform>::Get();

		bool bMatchedAllParams = true;
		for (TFieldIterator<FProperty> It(Function); It && (It->PropertyFlags & CPF_Parm); ++It)
		{
			FProperty* Property = *It;

			bool bFoundParam = false;
			for (int32 i = 0; !bFoundParam && (i < RemainingPins.Num()); ++i)
			{
				UEdGraphPin* PinMatch = RemainingPins[i];
				if (Property->GetFName() == PinMatch->PinName)
				{
					// @pushkin: disable type check
					{
						UEdGraphPin* PinToTry = FEdGraphUtilities::GetNetFromPin(PinMatch);

						if (FBPTerminal** Term = Context.NetMap.Find(PinToTry))
						{
							if ((*Term)->bIsLiteral && (PinMatch != LatentInfoPin))
							{
								FStructProperty* StructProperty = CastField<FStructProperty>(Property);
								if (StructProperty)
								{
									UScriptStruct* Struct = StructProperty->Struct;
									if (Struct != VectorStruct && Struct != RotatorStruct && Struct != TransformStruct)
									{
										// Ensure all literal struct terms can be imported if its empty
										if ((*Term)->Name.IsEmpty())
										{
											(*Term)->Name = TEXT("()");
										}

										int32 StructSize = Struct->GetStructureSize();
										[this, StructSize, StructProperty, Node, Term, &bMatchedAllParams]() {
											uint8* StructData = (uint8*)FMemory_Alloca(StructSize);
											StructProperty->InitializeValue(StructData);

											FImportTextErrorContext ErrorPipe(CompilerContext.MessageLog, Node);
											StructProperty->ImportText(*((*Term)->Name), StructData, 0, nullptr, &ErrorPipe);
											if (ErrorPipe.NumErrors > 0)
											{
												bMatchedAllParams = false;
											}
										}();
									}
								}
							}

							if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
							{
								LHSTerm = *Term;
							}
							else
							{
								FBPTerminal* RHSTerm = *Term;

								if (FBPTerminal** InterfaceTerm = InterfaceTermMap.Find(PinMatch))
								{
									UClass* InterfaceClass = CastChecked<UClass>(PinMatch->PinType.PinSubCategoryObject.Get());

									FBPTerminal* ClassTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
									ClassTerm->Name = InterfaceClass->GetName();
									ClassTerm->bIsLiteral = true;
									ClassTerm->Source = Node;
									ClassTerm->ObjectLiteral = InterfaceClass;
									ClassTerm->Type.PinCategory = UEdGraphSchema_K2::PC_Class;

									FBlueprintCompiledStatement& CastStatement = Context.AppendStatementForNode(Node);
									CastStatement.Type = InterfaceClass->HasAnyClassFlags(CLASS_Interface) ? KCST_CastObjToInterface : KCST_CastInterfaceToObj;
									CastStatement.LHS = *InterfaceTerm;
									CastStatement.RHS.Add(ClassTerm);
									CastStatement.RHS.Add(*Term);

									RHSTerm = *InterfaceTerm;
								}

								int32 ParameterIndex = RHSTerms.Add(RHSTerm);

								if (PinMatch == LatentInfoPin)
								{
									ThenExecPin = CompilerContext.GetSchema()->FindExecutionPin(*Node, EGPD_Output);

									if (ThenExecPin && (ThenExecPin->LinkedTo.Num() > 0))
									{
										LatentTargetNode = ThenExecPin->LinkedTo[0]->GetOwningNode();
									}

									if (LatentTargetNode)
									{
										LatentTargetParamIndex = ParameterIndex;
									}
								}
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
							bMatchedAllParams = false;
						}
					}

					bFoundParam = true;
					RemainingPins.RemoveAt(i);
				}
			}

			if (!bFoundParam)
			{
				CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("FindPinParameter_ErrorFmt", "Could not find a pin for the parameter {0} of {1} on @@"), FText::FromString(Property->GetName()), FText::FromString(Function->GetName())).ToString(), Node);
				bMatchedAllParams = false;
			}
		}

		if (RemainingPins.Num() > 0)
		{
			const bool bIsVariadic = Function->HasMetaData(FBlueprintMetadata::MD_Variadic);
			if (bIsVariadic)
			{
				for (UEdGraphPin* RemainingPin : RemainingPins)
				{
					if (RemainingPin->LinkedTo.Num() == 0)
					{
						CompilerContext.MessageLog.Error(*LOCTEXT("UnlinkedVariadicPin_Error", "The variadic pin @@ must be connected. Connect something to @@.").ToString(), RemainingPin, RemainingPin->GetOwningNodeUnchecked());
						continue;
					}

					UEdGraphPin* PinToTry = FEdGraphUtilities::GetNetFromPin(RemainingPin);
					if (FBPTerminal** Term = Context.NetMap.Find(PinToTry))
					{
						FBPTerminal* RHSTerm = *Term;
						RHSTerms.Add(RHSTerm);
					}
					else
					{
						CompilerContext.MessageLog.Error(*LOCTEXT("ResolveTermVariadic_Error", "Failed to resolve variadic term passed into @@").ToString(), RemainingPin);
						bMatchedAllParams = false;
					}
				}
			}
			else
			{
				for (const UEdGraphPin* RemainingPin : RemainingPins)
				{
					CompilerContext.MessageLog.Error(*FText::Format(LOCTEXT("PinMismatchParameter_ErrorFmt", "Pin @@ named {0} doesn't match any parameters of function {1}"), FText::FromName(RemainingPin->PinName), FText::FromString(Function->GetName())).ToString(), RemainingPin);
				}
			}
		}

		if (NumErrorsAtStart == CompilerContext.MessageLog.NumErrors)
		{
			TArray<FBPTerminal*> ContextTerms;
			if (SelfPin)
			{
				const bool bIsConstSelfContext = Context.IsConstFunction();
				const bool bIsNonConstFunction = !Function->HasAnyFunctionFlags(FUNC_Const | FUNC_Static);
				const bool bEnforceConstCorrectness = Context.EnforceConstCorrectness();
				auto CheckAndAddSelfTermLambda = [this, &Node, &ContextTerms, bIsConstSelfContext, bIsNonConstFunction, bEnforceConstCorrectness](FBPTerminal* Target) {
					bool bIsSelfTerm = true;
					if (Target != nullptr)
					{
						const UEdGraphPin* SourcePin = Target->SourcePin;
						bIsSelfTerm = (SourcePin == nullptr || CompilerContext.GetSchema()->IsSelfPin(*SourcePin));
					}
					if (bIsSelfTerm && bIsConstSelfContext && bIsNonConstFunction)
					{
						if (Target != nullptr)
						{
							if (bEnforceConstCorrectness)
							{
								CompilerContext.MessageLog.Error(*LOCTEXT("NonConstFunctionCallOnReadOnlyTarget_Error", "Function @@ can modify state and cannot be called on @@ because it is a read-only Target in this context").ToString(), Node, Target->Source);
							}
							else
							{
								CompilerContext.MessageLog.Warning(*LOCTEXT("NonConstFunctionCallOnReadOnlyTarget_Warning", "Function @@ can modify state and should not be called on @@ because it is considered to be a read-only Target in this context").ToString(), Node, Target->Source);
							}
						}
						else
						{
							if (bEnforceConstCorrectness)
							{
								CompilerContext.MessageLog.Error(*LOCTEXT("NonConstFunctionCallOnReadOnlySelfScope_Error", "Function @@ can modify state and cannot be called on 'self' because it is a read-only Target in this context").ToString(), Node);
							}
							else
							{
								CompilerContext.MessageLog.Warning(*LOCTEXT("NonConstFunctionCallOnReadOnlySelfScope_Warning", "Function @@ can modify state and should not be called on 'self' because it is considered to be a read-only Target in this context").ToString(), Node);
							}
						}
					}

					ContextTerms.Add(Target);
				};

				if (SelfPin->LinkedTo.Num() > 0)
				{
					for (int32 i = 0; i < SelfPin->LinkedTo.Num(); i++)
					{
						FBPTerminal** pContextTerm = Context.NetMap.Find(SelfPin->LinkedTo[i]);
						if (ensureMsgf(pContextTerm != nullptr, TEXT("'%s' is missing a target input - if this is a server build, the input may be a cosmetic only property which was discarded (if this is the case, and this is expecting component variable try resaving.)"), *Node->GetPathName()))
						{
							CheckAndAddSelfTermLambda(*pContextTerm);
						}
					}
				}
				else
				{
					FBPTerminal** pContextTerm = Context.NetMap.Find(SelfPin);
					CheckAndAddSelfTermLambda((pContextTerm != nullptr) ? *pContextTerm : nullptr);
				}
			}

			UEdGraphNode** pSrcEventNode = nullptr;
			if (!bIsLatent)
			{
				pSrcEventNode = CompilerContext.CallsIntoUbergraph.Find(Node);
			}

			bool bInlineEventCall = false;
			bool bEmitInstrumentPushState = false;
			FName EventName = NAME_None;

			FBlueprintCompiledStatement* LatentStatement = nullptr;
			for (FBPTerminal* Target : ContextTerms)
			{
				FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
				Statement.FunctionToCall = Function;
				Statement.FunctionContext = Target;
				Statement.Type = KCST_CallFunction;
				Statement.bIsInterfaceContext = IsCalledFunctionFromInterface(Node);
				Statement.bIsParentContext = IsCalledFunctionFinal(Node);

				Statement.LHS = LHSTerm;
				Statement.RHS = RHSTerms;

				if (!bIsLatent)
				{
					// Fixup ubergraph calls
					if (pSrcEventNode)
					{
						UEdGraphPin* ExecOut = CompilerContext.GetSchema()->FindExecutionPin(**pSrcEventNode, EGPD_Output);

						check(CompilerContext.UbergraphContext);
						CompilerContext.UbergraphContext->GotoFixupRequestMap.Add(&Statement, ExecOut);
						Statement.UbergraphCallIndex = 0;
					}
				}
				else
				{
					if (LatentTargetNode && (Target == ContextTerms.Last()))
					{
						check(LatentTargetParamIndex != INDEX_NONE);
						Statement.UbergraphCallIndex = LatentTargetParamIndex;
						Context.GotoFixupRequestMap.Add(&Statement, ThenExecPin);
						LatentStatement = &Statement;
					}
				}

				AdditionalCompiledStatementHandling(Context, Node, Statement);

				if (Statement.Type == KCST_CallFunction && Function->HasAnyFunctionFlags(FUNC_Delegate))
				{
					CompilerContext.MessageLog.Error(*LOCTEXT("CallingDelegate_Error", "@@ is trying to call a delegate function - delegates cannot be called directly").ToString(), Node);
					Statement.Type = KCST_CallDelegate;
				}
			}

			if (bIsLatent)
			{
				FBlueprintCompiledStatement& PopStatement = Context.AppendStatementForNode(Node);
				PopStatement.Type = KCST_EndOfThread;
			}
			else
			{
				if (!IsCalledFunctionPure(Node))
				{
					GenerateSimpleThenGoto(Context, *Node);
				}
			}
		}
	}
	else
	{
		FString WarningMessage = FText::Format(LOCTEXT("FindFunction_ErrorFmt", "Could not find the function '{0}' called from @@"), FText::FromString(GetFunctionNameFromNode(Node).ToString())).ToString();
		CompilerContext.MessageLog.Warning(*WarningMessage, Node);
	}
}

UClass* FPsDataHandler_CallFunction::GetCallingContext(FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	UClass* SearchScope = Context.NewClass;
	UK2Node_CallFunction* CallFuncNode = Cast<UK2Node_CallFunction>(Node);
	if (CallFuncNode && CallFuncNode->bIsFinalFunction)
	{
		if (UK2Node_CallParentFunction* ParentCall = Cast<UK2Node_CallParentFunction>(Node))
		{
			const FName FuncName = CallFuncNode->FunctionReference.GetMemberName();
			UClass* SearchContext = Context.NewClass->GetSuperClass();

			UFunction* ParentFunc = nullptr;
			if (SearchContext)
			{
				ParentFunc = SearchContext->FindFunctionByName(FuncName);
			}

			return ParentFunc ? ParentFunc->GetOuterUClass() : nullptr;
		}
		else
		{
			return CallFuncNode->FunctionReference.GetMemberParentClass(CallFuncNode->GetBlueprintClassFromNode());
		}
	}
	else
	{
		if (UEdGraphPin* SelfPin = CompilerContext.GetSchema()->FindSelfPin(*Node, EGPD_Input))
		{
			SearchScope = Cast<UClass>(Context.GetScopeFromPinType(SelfPin->PinType, Context.NewClass));
		}
	}

	return SearchScope;
}

UClass* FPsDataHandler_CallFunction::GetTrueCallingClass(FKismetFunctionContext& Context, UEdGraphPin* SelfPin)
{
	if (SelfPin)
	{
		FEdGraphPinType& Type = SelfPin->PinType;
		if ((Type.PinCategory == UEdGraphSchema_K2::PC_Object) || (Type.PinCategory == UEdGraphSchema_K2::PC_Class) || (Type.PinCategory == UEdGraphSchema_K2::PC_Interface))
		{
			if (!Type.PinSubCategory.IsNone() && (Type.PinSubCategory != UEdGraphSchema_K2::PSC_Self))
			{
				return Cast<UClass>(Type.PinSubCategoryObject.Get());
			}
		}
	}
	return Context.NewClass;
}

// Copied from FKCPushModelHelpers::ConstructMarkDirtyNodeForProperty
UEdGraphNode* FPsDataHandler_CallFunction::ConstructMarkDirtyNodeForProperty(FKismetFunctionContext& Context, FProperty* RepProperty, UEdGraphPin* PropertyObjectPin)
{
	static const FName MarkPropertyDirtyFuncName(TEXT("MarkPropertyDirtyFromRepIndex"));
	static const FName ObjectPinName(TEXT("Object"));
	static const FName RepIndexPinName(TEXT("RepIndex"));
	static const FName PropertyNamePinName(TEXT("PropertyName"));

	if (!ensure(RepProperty) || !ensure(PropertyObjectPin))
	{
		return nullptr;
	}

	UClass* OwningClass = RepProperty->GetTypedOwner<UClass>();
	if (!ensure(OwningClass))
	{
		return nullptr;
	}

	if (RepProperty->GetOffset_ForGC() == 0)
	{
		if (OwningClass->ClassGeneratedBy == Context.Blueprint && Context.NewClass && Context.NewClass != OwningClass)
		{
			OwningClass = Context.NewClass;
			RepProperty = FindFieldChecked<FProperty>(OwningClass, RepProperty->GetFName());
		}

		if (RepProperty->GetOffset_ForGC() == 0)
		{
			if (UBlueprint* Blueprint = Cast<UBlueprint>(OwningClass->ClassGeneratedBy))
			{
				if (UClass* UseClass = Blueprint->GeneratedClass)
				{
					OwningClass = UseClass;
					RepProperty = FindFieldChecked<FProperty>(OwningClass, RepProperty->GetFName());
				}
			}
		}
	}

	ensureAlwaysMsgf(RepProperty->GetOffset_ForGC() != 0,
		TEXT("Class does not have Property Offsets setup. This will cause issues with Push Model. Blueprint=%s, Class=%s, Property=%s"),
		*Context.Blueprint->GetPathName(), *OwningClass->GetPathName(), *RepProperty->GetName());

	if (!OwningClass->HasAnyClassFlags(CLASS_ReplicationDataIsSetUp))
	{
		OwningClass->SetUpRuntimeReplicationData();
	}

	UK2Node_CallFunction* MarkPropertyDirtyNode = Context.SourceGraph->CreateIntermediateNode<UK2Node_CallFunction>();
	MarkPropertyDirtyNode->FunctionReference.SetExternalMember(MarkPropertyDirtyFuncName, UNetPushModelHelpers::StaticClass());
	MarkPropertyDirtyNode->AllocateDefaultPins();

	UEdGraphPin* RepIndexPin = MarkPropertyDirtyNode->FindPinChecked(RepIndexPinName);
	RepIndexPin->DefaultValue = FString::FromInt(RepProperty->RepIndex);

	UEdGraphPin* PropertyNamePin = MarkPropertyDirtyNode->FindPinChecked(PropertyNamePinName);
	PropertyNamePin->DefaultValue = RepProperty->GetFName().ToString();

	if (PropertyObjectPin->LinkedTo.Num() > 0)
	{
		PropertyObjectPin = PropertyObjectPin->LinkedTo[0];
	}
	else
	{
		UK2Node_Self* SelfNode = Context.SourceGraph->CreateIntermediateNode<UK2Node_Self>();
		SelfNode->AllocateDefaultPins();
		PropertyObjectPin = SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	}

	UEdGraphPin* ObjectPin = MarkPropertyDirtyNode->FindPinChecked(ObjectPinName);
	ObjectPin->MakeLinkTo(PropertyObjectPin);

	return MarkPropertyDirtyNode;
}

void FPsDataHandler_CallFunction::RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	if (UFunction* Function = FindFunction(Context, Node))
	{
		TArray<FName> DefaultToSelfParamNames;
		TArray<FName> RequiresSetValue;

		if (Function->HasMetaData(FBlueprintMetadata::MD_DefaultToSelf))
		{
			const FName DefaltToSelfPinName = *Function->GetMetaData(FBlueprintMetadata::MD_DefaultToSelf);

			DefaultToSelfParamNames.Add(DefaltToSelfPinName);
		}
		if (Function->HasMetaData(FBlueprintMetadata::MD_WorldContext))
		{
			UEdGraphSchema_K2 const* K2Schema = CompilerContext.GetSchema();
			const bool bHasIntrinsicWorldContext = !K2Schema->IsStaticFunctionGraph(Context.SourceGraph) && FBlueprintEditorUtils::ImplementsGetWorld(Context.Blueprint);

			const FName WorldContextPinName = *Function->GetMetaData(FBlueprintMetadata::MD_WorldContext);

			if (bHasIntrinsicWorldContext)
			{
				DefaultToSelfParamNames.Add(WorldContextPinName);
			}
			else if (!Function->HasMetaData(FBlueprintMetadata::MD_CallableWithoutWorldContext))
			{
				RequiresSetValue.Add(WorldContextPinName);
			}
		}

		for (UEdGraphPin* Pin : Node->Pins)
		{
			const bool bIsConnected = (Pin->LinkedTo.Num() != 0);

			if (!bIsConnected && (Pin->DefaultObject == nullptr))
			{
				if (DefaultToSelfParamNames.Contains(Pin->PinName) && FKismetCompilerUtilities::ValidateSelfCompatibility(Pin, Context))
				{
					ensure(Pin->PinType.PinSubCategoryObject != nullptr);
					ensure((Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object) || (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface));

					FBPTerminal* Term = Context.RegisterLiteral(Pin);
					Term->Type.PinSubCategory = UEdGraphSchema_K2::PN_Self;
					Context.NetMap.Add(Pin, Term);
				}
				else if (RequiresSetValue.Contains(Pin->PinName))
				{
					CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "PinMustHaveConnection_Error", "Pin @@ must have a connection").ToString(), Pin);
				}
			}
		}
	}

	for (UEdGraphPin* Pin : Node->Pins)
	{
		if ((Pin->Direction != EGPD_Input) || (Pin->LinkedTo.Num() == 0))
		{
			continue;
		}

		if (((Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface) && (Pin->LinkedTo[0]->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)) ||
			((Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object) && (Pin->LinkedTo[0]->PinType.PinCategory == UEdGraphSchema_K2::PC_Interface)))
		{
			FBPTerminal* InterfaceTerm = Context.CreateLocalTerminal();
			InterfaceTerm->CopyFromPin(Pin, Context.NetNameMap->MakeValidName(Pin, TEXT("CastInput")));
			InterfaceTerm->Source = Node;

			InterfaceTermMap.Add(Pin, InterfaceTerm);
		}
	}

	FNodeHandlingFunctor::RegisterNets(Context, Node);
}

void FPsDataHandler_CallFunction::RegisterNet(FKismetFunctionContext& Context, UEdGraphPin* Net)
{
	FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));
	Context.NetMap.Add(Net, Term);
}

UFunction* FPsDataHandler_CallFunction::FindFunction(FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	UClass* CallingContext = GetCallingContext(Context, Node);

	if (CallingContext)
	{
		if (!FBlueprintCompilationManager::IsGeneratedClassLayoutReady())
		{
			CallingContext = FBlueprintEditorUtils::GetMostUpToDateClass(CallingContext);
		}

		const FName FunctionName = GetFunctionNameFromNode(Node);
		return CallingContext->FindFunctionByName(FunctionName);
	}

	return nullptr;
}

static bool FindMatchingReferencedNetPropertyAndPin(TArray<UEdGraphPin*>& RemainingPins, FProperty* FunctionProperty, FProperty*& NetProperty, UEdGraphPin*& PropertyObjectPin)
{
	NetProperty = nullptr;
	PropertyObjectPin = nullptr;

	if (UNLIKELY(FunctionProperty->HasAllPropertyFlags(CPF_OutParm | CPF_ReferenceParm) && !FunctionProperty->HasAnyPropertyFlags(CPF_ReturnParm | CPF_ConstParm)))
	{
		for (int32 i = 0; i < RemainingPins.Num(); ++i)
		{
			if (FunctionProperty->GetFName() == RemainingPins[i]->PinName)
			{
				UEdGraphPin* ParamPin = RemainingPins[i];
				RemainingPins.RemoveAtSwap(i);
				if (UEdGraphPin* PinToTry = FEdGraphUtilities::GetNetFromPin(ParamPin))
				{
					if (UK2Node_VariableGet* GetPropertyNode = Cast<UK2Node_VariableGet>(PinToTry->GetOwningNode()))
					{
						FProperty* ToCheck = GetPropertyNode->GetPropertyForVariable();
						if (UNLIKELY(ToCheck && ToCheck->HasAnyPropertyFlags(CPF_Net)))
						{
							NetProperty = ToCheck;
							PropertyObjectPin = GetPropertyNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
							return true;
						}
					}
				}

				return false;
			}
		}
	}

	return false;
}

void FPsDataHandler_CallFunction::Transform(FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	UK2Node_CallFunction* CallFuncNode = Cast<UK2Node_CallFunction>(Node);
	if (!CallFuncNode)
	{
		return;
	}

	const bool bIsPure = CallFuncNode->bIsPureFunc;
	bool bIsPureAndNoUsedOutputs = false;
	if (bIsPure)
	{
		bool bAnyOutputsUsed = false;
		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* Pin = Node->Pins[PinIndex];
			if ((Pin->Direction == EGPD_Output) && (Pin->LinkedTo.Num() > 0))
			{
				bAnyOutputsUsed = true;
				break;
			}
		}

		if (!bAnyOutputsUsed)
		{
			bIsPureAndNoUsedOutputs = true;
		}
	}

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	if (UFunction* Function = FindFunction(Context, Node))
	{
		const bool bIsLatent = Function->HasMetaData(FBlueprintMetadata::MD_Latent);
		if (bIsLatent)
		{
			UEdGraphPin* OldOutPin = K2Schema->FindExecutionPin(*CallFuncNode, EGPD_Output);

			if ((OldOutPin != nullptr) && (OldOutPin->LinkedTo.Num() > 0))
			{
				UK2Node_ExecutionSequence* DummyNode = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(CallFuncNode);
				DummyNode->AllocateDefaultPins();

				UEdGraphPin* NewInPin = K2Schema->FindExecutionPin(*DummyNode, EGPD_Input);
				UEdGraphPin* NewOutPin = K2Schema->FindExecutionPin(*DummyNode, EGPD_Output);

				if ((NewInPin != nullptr) && (NewOutPin != nullptr))
				{
					CompilerContext.MessageLog.NotifyIntermediatePinCreation(NewOutPin, OldOutPin);

					while (OldOutPin->LinkedTo.Num() > 0)
					{
						UEdGraphPin* LinkedPin = OldOutPin->LinkedTo[0];

						LinkedPin->BreakLinkTo(OldOutPin);
						LinkedPin->MakeLinkTo(NewOutPin);
					}

					OldOutPin->MakeLinkTo(NewInPin);
				}
			}
		}

		if (!bIsPureAndNoUsedOutputs && Function->NumParms > 0 && Function->HasAllFunctionFlags(FUNC_HasOutParms))
		{
			TArray<UEdGraphPin*> RemainingPins;
			RemainingPins.Reserve(Node->Pins.Num());
			for (UEdGraphPin* Pin : Node->Pins)
			{
				if (Pin->Direction == EGPD_Input &&
					!Pin->PinType.bIsConst &&
					Pin->PinType.bIsReference &&
					Pin->LinkedTo.Num() > 0)
				{
					RemainingPins.Add(Pin);
				}
			}

			if (RemainingPins.Num() > 0)
			{
				FProperty* NetProperty = nullptr;
				UEdGraphPin* PropertyObjectPin = nullptr;
				UEdGraphPin* OldThenPin = CallFuncNode->GetThenPin();

				for (TFieldIterator<FProperty> It(Function); It; ++It)
				{
					if (FindMatchingReferencedNetPropertyAndPin(RemainingPins, *It, NetProperty, PropertyObjectPin))
					{
						if (bIsPure)
						{
							break;
						}

						if (UEdGraphNode* MarkPropertyDirtyNode = ConstructMarkDirtyNodeForProperty(Context, NetProperty, PropertyObjectPin))
						{
							UEdGraphPin* NewThenPin = MarkPropertyDirtyNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
							UEdGraphPin* NewInPin = MarkPropertyDirtyNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute);

							if (ensure(NewThenPin) && ensure(NewInPin))
							{
								if (OldThenPin)
								{
									NewThenPin->CopyPersistentDataFromOldPin(*OldThenPin);
									OldThenPin->BreakAllPinLinks();
									OldThenPin->MakeLinkTo(NewInPin);

									OldThenPin = NewThenPin;
								}
								else
								{
									if (bIsLatent)
									{
										CompilerContext.MessageLog.Warning(*NSLOCTEXT("KismetCompiler", "LatentPushModel_Warning", "@@ is a latent node with references to a net property. The property may not be marked dirty in the correct frame.").ToString(), Node);
									}

									UEdGraphPin* OldInPin = CallFuncNode->FindPin(UEdGraphSchema_K2::PN_Execute);
									if (OldInPin)
									{
										NewInPin->CopyPersistentDataFromOldPin(*OldInPin);
										OldInPin->BreakAllPinLinks();

										NewThenPin->MakeLinkTo(OldInPin);
										OldThenPin = NewThenPin;
									}
								}
							}
						}
					}

					if (RemainingPins.Num() == 0)
					{
						break;
					}
				}
			}
		}
	}
}

void FPsDataHandler_CallFunction::Compile(FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	check(NULL != Node);

	if (!IsCalledFunctionPure(Node))
	{
		UEdGraphPin* ExecTriggeringPin = CompilerContext.GetSchema()->FindExecutionPin(*Node, EGPD_Input);
		if (ExecTriggeringPin == nullptr)
		{
			CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "NoValidExecutionPinForCallFunc_Error", "@@ must have a valid execution pin").ToString(), Node);
			return;
		}
		else if (ExecTriggeringPin->LinkedTo.Num() == 0)
		{
			CompilerContext.MessageLog.Warning(*NSLOCTEXT("KismetCompiler", "NodeNeverExecuted_Warning", "@@ will never be executed").ToString(), Node);
			return;
		}
	}

	UEdGraphPin* SelfPin = CompilerContext.GetSchema()->FindSelfPin(*Node, EGPD_Input);
	if (SelfPin && (SelfPin->LinkedTo.Num() == 0))
	{
		FEdGraphPinType SelfType;
		SelfType.PinCategory = UEdGraphSchema_K2::PC_Object;
		SelfType.PinSubCategory = UEdGraphSchema_K2::PSC_Self;

		if (!CompilerContext.GetSchema()->ArePinTypesCompatible(SelfType, SelfPin->PinType, Context.NewClass) && (SelfPin->DefaultObject == nullptr))
		{
			CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "PinMustHaveConnectionPruned_Error", "Pin @@ must have a connection.  Self pins cannot be connected to nodes that are culled.").ToString(), SelfPin);
		}
	}

	CreateFunctionCallStatement(Context, Node, SelfPin);
}

void FPsDataHandler_CallFunction::CheckIfFunctionIsCallable(UFunction* Function, FKismetFunctionContext& Context, UEdGraphNode* Node)
{
	if (!Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) && (Function->GetOuter() != Context.NewClass))
	{
		if (!IsCalledFunctionFinal(Node) && Function->GetName().Find(UEdGraphSchema_K2::FN_ExecuteUbergraphBase.ToString()))
		{
			CompilerContext.MessageLog.Error(*FText::Format(NSLOCTEXT("KismetCompiler", "ShouldNotCallFromBlueprint_ErrorFmt", "Function '{0}' called from @@ should not be called from a Blueprint"), FText::FromString(Function->GetName())).ToString(), Node);
		}
	}
}

FName FPsDataHandler_CallFunction::GetFunctionNameFromNode(UEdGraphNode* Node) const
{
	UK2Node_CallFunction* CallFuncNode = Cast<UK2Node_CallFunction>(Node);
	if (CallFuncNode)
	{
		return CallFuncNode->FunctionReference.GetMemberName();
	}
	else
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("KismetCompiler", "UnableResolveFunctionName_Error", "Unable to resolve function name for @@").ToString(), Node);
		return NAME_None;
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
