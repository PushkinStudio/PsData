// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsDataEvent.h"

#include "PsData.h"
#include "PsDataField.h"

#include "Async/Async.h"

const FString UPsDataEvent::Added(TEXT("Added"));
const FString UPsDataEvent::Removing(TEXT("Removing"));
const FString UPsDataEvent::Changed(TEXT("Changed"));

UPsDataEvent::UPsDataEvent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Type(TEXT("Unknown"))
	, Target(nullptr)
	, bBubbles(false)
	, bStopImmediate(false)
	, bStop(false)
{
}

void UPsDataEvent::DispatchChange(UPsData* Instance, TSharedPtr<const FDataField> Field)
{
	if (Field->Meta.bEvent)
	{
		Instance->Broadcast(UPsDataEvent::ConstructEvent(Field->GenerateChangePropertyEventName(), Field->Meta.bBubbles));
	}

	if (!FDataReflectionTools::FPsDataFriend::IsChanged(Instance))
	{
		FDataReflectionTools::FPsDataFriend::SetIsChanged(Instance, true);
		TWeakObjectPtr<UPsData> InstanceWeakPtr(Instance);
		AsyncTask(ENamedThreads::GameThread, [InstanceWeakPtr]() {
			if (InstanceWeakPtr.IsValid())
			{
				FDataReflectionTools::FPsDataFriend::SetIsChanged(InstanceWeakPtr.Get(), false);
				InstanceWeakPtr->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::Changed, false));
			}
		});
	}
}

UPsDataEvent* UPsDataEvent::ConstructEvent(FString EventType, bool bEventBubbles)
{
	UPsDataEvent* Event = NewObject<UPsDataEvent>();

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

UPsData* UPsDataEvent::GetTarget_Mutable() const
{
	return Target;
}
