// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsEvent.h"

UPsEvent::UPsEvent(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, Type(TEXT("Unknown"))
, Target(nullptr)
, bBubbles(false)
, bStopImmediate(false)
, bStop(false)
{
	
}

UPsEvent* UPsEvent::ConstructEvent(FString EventType, bool bEventBubbles)
{
	UPsEvent* Event = NewObject<UPsEvent>();
	
	Event->Type = EventType;
	Event->bBubbles = bEventBubbles;
	
	return Event;
}

FString UPsEvent::GetType() const
{
	return Type;
}

UPsData* UPsEvent::GetTarget() const
{
	return Target;
}

bool UPsEvent::IsBubbles() const
{
	return bBubbles;
}

void UPsEvent::StopImmediatePropagation()
{
	bStopImmediate = true;
	bStop = true;
}

void UPsEvent::StopPropagation()
{
	bStop = true;
}
