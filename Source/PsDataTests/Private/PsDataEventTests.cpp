// Copyright 2015-2026 MY.GAMES. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataFieldEventsTest, "PsData.Events.Field",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataFieldEventsTest::RunTest(const FString& Parameters)
{
	const auto Character = NewObject<UPsDataTestCharacter>();
	FPsDataBindCollection Binds;

	int32 HealthEvents = 0;
	int32 PreviousHealth = INDEX_NONE;
	Binds.Add(Character->Health.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent* Event) {
		++HealthEvents;
		PreviousHealth = Character->Health.GetEventStorage(Event)->GetPreviousValue();
	})));

	Character->Health = 60;
	TestEqual(TEXT("Event meta: the change is broadcast synchronously"), HealthEvents, 1);
	TestEqual(TEXT("EventStorage meta: the event carries the previous value"), PreviousHealth, 100);

	Character->Health = 60;
	TestEqual(TEXT("Assigning the same value broadcasts nothing"), HealthEvents, 1);

	int32 KillsEvents = 0;
	int32 KillsEventsForced = 0;
	Binds.Add(Character->Kills.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++KillsEvents; })));

	Character->Kills = 1;
	TestEqual(TEXT("No Event meta: a plain listener hears nothing"), KillsEvents, 0);

	// One IgnoreFieldMeta listener makes the field broadcast, and then every listener of that event hears it
	Binds.Add(Character->Kills.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++KillsEventsForced; }), EDataBindFlags::IgnoreFieldMeta));
	Character->Kills = 2;
	TestEqual(TEXT("No Event meta: an IgnoreFieldMeta listener hears it"), KillsEventsForced, 1);
	TestEqual(TEXT("No Event meta: plain listeners ride along once the event is broadcast"), KillsEvents, 1);

	Binds.Unbind();
	Character->Health = 1;
	Character->Kills = 3;
	TestEqual(TEXT("FPsDataBindCollection::Unbind detaches every listener"), HealthEvents + KillsEventsForced, 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataBubblingEventsTest, "PsData.Events.Bubbling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataBubblingEventsTest::RunTest(const FString& Parameters)
{
	const auto Root = PsDataTest::MakeModel();
	const auto Alice = Root->Battle->Characters[TEXT("alice")];
	FPsDataBindCollection Binds;

	// Point.X is Event+Bubbles: a listener on the battle hears a change made three levels down
	int32 HeardOnBattle = 0;
	const UPsData* Target = nullptr;
	Binds.Add(Root->Battle->Bind(UPsDataTestPoint::GetXChangedEventName(), FPsDataDelegate::CreateLambda([&](UPsDataEvent* Event) {
		++HeardOnBattle;
		Target = Event->GetTarget();
	})));

	Alice->Location->X = 5;
	TestEqual(TEXT("Bubbling event reaches an ancestor"), HeardOnBattle, 1);
	TestTrue(TEXT("Event target is the data that changed"), Target == *Alice->Location);

	Binds.Add(Alice->Location->X.Bind(FPsDataDelegate::CreateLambda([](UPsDataEvent* Event) {
		Event->StopPropagation();
	})));
	Alice->Location->X = 6;
	TestEqual(TEXT("StopPropagation keeps the event from bubbling"), HeardOnBattle, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataCollectionEventsTest, "PsData.Events.Collection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataCollectionEventsTest::RunTest(const FString& Parameters)
{
	const auto Root = PsDataTest::MakeModel();
	FPsDataBindCollection Binds;

	TArray<FString> Added;
	TArray<FString> Removed;
	Binds.Add(Root->Battle->Characters->Bind(UPsDataEvent::Added, FPsDataDelegate::CreateLambda([&](UPsDataEvent* Event) {
		Added.Add(Event->GetTarget()->GetDataKey());
	})));
	Binds.Add(Root->Battle->Characters->Bind(UPsDataEvent::Removed, FPsDataDelegate::CreateLambda([&](UPsDataEvent* Event) {
		Removed.Add(Event->GetTarget()->GetDataKey());
	})));

	Root->Battle->Characters->Add(TEXT("carol"), NewObject<UPsDataTestCharacter>());
	TestTrue(TEXT("Added fires with the new element as target"), Added == TArray<FString>{TEXT("carol")});

	Root->Battle->Characters->Remove(TEXT("bob"));
	TestTrue(TEXT("Removed fires with the removed element as target"), Removed == TArray<FString>{TEXT("bob")});
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataDeferredEventsTest, "PsData.Events.Deferred",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataDeferredEventsTest::RunTest(const FString& Parameters)
{
	const auto Character = NewObject<UPsDataTestCharacter>();
	FPsDataBindCollection Binds;

	// The generic "Changed" event is coalesced: one broadcast per object per game-thread task flush
	int32 Changed = 0;
	Binds.Add(Character->Bind(UPsDataEvent::Changed, FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++Changed; })));

	Character->Health = 10;
	Character->Health = 20;
	Character->Name = TEXT("Alice");
	TestEqual(TEXT("Changed is not broadcast inline"), Changed, 0);

	PsDataTest::FlushDeferred();
	TestEqual(TEXT("Changed is broadcast once for several edits"), Changed, 1);

	// DEFERRED_EVENT_PROCESSING holds listeners back until the scope ends; NonDeferred ones still run inline
	int32 Deferred = 0;
	int32 Inline = 0;
	Binds.Add(Character->Health.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++Deferred; })));
	Binds.Add(Character->Health.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++Inline; }), EDataBindFlags::NonDeferred));
	{
		DEFERRED_EVENT_PROCESSING();

		Character->Health = 30;
		TestEqual(TEXT("Inside the scope: default listeners wait"), Deferred, 0);
		TestEqual(TEXT("Inside the scope: NonDeferred listeners run"), Inline, 1);
	}
	TestEqual(TEXT("Leaving the scope delivers the held events"), Deferred, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataPromiseTest, "PsData.Events.Promise",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataPromiseTest::RunTest(const FString& Parameters)
{
	// A promise runs listeners bound before Resolve on Resolve, and listeners bound after it immediately
	TPsDataPromise<int32> Promise;
	TArray<int32> Received;
	Promise.Bind(TPsDataPromise<int32>::FPsDataPromiseDelegate::CreateLambda([&](const int32& Value) { Received.Add(Value); }));
	TestEqual(TEXT("Nothing runs before Resolve"), Received.Num(), 0);

	Promise.Resolve(42);
	Promise.Bind(TPsDataPromise<int32>::FPsDataPromiseDelegate::CreateLambda([&](const int32& Value) { Received.Add(Value + 1); }));
	TestTrue(TEXT("Early and late listeners both see the value"), Received == TArray<int32>{42, 43});

	// The same contract, without a value: what FDataDelegates::PostDataModuleInitPromise is
	TestTrue(TEXT("The data module has resolved its init promise"), PsDataTools::FDataReflection::IsCompiled());
	bool bLate = false;
	FDataDelegates::PostDataModuleInitPromise.Bind(FSimpleDelegate::CreateLambda([&]() { bLate = true; }));
	TestTrue(TEXT("Binding to a resolved simple promise runs at once"), bLate);
	return true;
}

#endif
