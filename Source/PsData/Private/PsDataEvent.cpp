// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsDataEvent.h"

#include "PsData.h"
#include "PsDataField.h"

#include "UObject/Package.h"

int32 FPsDataEventScopeGuard::Index = 0;
TArray<FPsDataEventScopeGuardCallback> FPsDataEventScopeGuard::Callbacks;

FPsDataEventScopeGuard::FPsDataEventScopeGuard()
{
	++Index;
}

FPsDataEventScopeGuard::~FPsDataEventScopeGuard()
{
	--Index;
	check(Index >= 0);

	while (Index == 0 && Callbacks.Num() > 0)
	{
		Invoke();
	}
}

void FPsDataEventScopeGuard::Invoke()
{
	auto CallbacksCopy = Callbacks;
	Callbacks.Reset();

	for (int32 i = 0; i < CallbacksCopy.Num(); ++i)
	{
		CallbacksCopy[i]();
	}
}

void FPsDataEventScopeGuard::AddCallback(FPsDataEventScopeGuardCallback Function)
{
	Callbacks.Add(Function);
}

bool FPsDataEventScopeGuard::IsGuarded()
{
	return Index > 0;
}

const FString UPsDataEvent::Added(TEXT("Added"));
const FString UPsDataEvent::AddedToParent(TEXT("AddedToParent"));
const FString UPsDataEvent::Removed(TEXT("Removed"));
const FString UPsDataEvent::RemovedFromParent(TEXT("RemovedFromParent"));
const FString UPsDataEvent::Changed(TEXT("Changed"));
const FString UPsDataEvent::NameChanged(TEXT("NameChanged"));

UPsDataEvent::UPsDataEvent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Type(TEXT("Unknown"))
	, Target(nullptr)
	, bBubbles(false)
	, bStopImmediate(false)
	, bStop(false)
{
}

UPsDataEvent* UPsDataEvent::ConstructEvent(FString EventType, bool bEventBubbles, UClass* EventClass)
{
	if (!EventClass)
	{
		EventClass = UPsDataEvent::StaticClass();
	}

	UPsDataEvent* Event = NewObject<UPsDataEvent>(GetTransientPackage(), EventClass);

	Event->Type = EventType;
	Event->bBubbles = bEventBubbles;

	return Event;
}

const UPsData* UPsDataEvent::GetTarget() const
{
	return Target;
}

const FString& UPsDataEvent::GetType() const
{
	return Type;
}

bool UPsDataEvent::IsBubbles() const
{
	return bBubbles;
}

void UPsDataEvent::StopImmediatePropagation()
{
	bStopImmediate = true;
	bStop = true;
}

void UPsDataEvent::StopPropagation()
{
	bStop = true;
}

DEFINE_FUNCTION(UPsDataEventFunctionLibrary::execGetEventTarget)
{
	P_GET_OBJECT(UPsDataEvent, Event);
	P_GET_OBJECT_REF(UPsData, Out);
	P_FINISH;
	P_NATIVE_BEGIN;
	Out = Event->Target;
	P_NATIVE_END;
}

void FCustomThunkTemplates_PsDataEvent::GetEventTarget(UPsDataEvent* Event, UPsData*& Target)
{
	Target = Event->Target;
}