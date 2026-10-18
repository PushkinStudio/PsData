// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataReflectionTest, "PsData.Reflection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataReflectionTest::RunTest(const FString& Parameters)
{
	// Compiled once, at PostEngineInit, from the CDOs of every UPsData subclass
	TestTrue(TEXT("Reflection compiled"), PsDataTools::FDataReflection::IsCompiled());

	const auto Fields = PsDataTools::FDataReflection::GetFieldsByClass(UPsDataTestCharacter::StaticClass());
	if (!TestNotNull(TEXT("Character fields registered"), Fields))
	{
		return false;
	}
	TestEqual(TEXT("Character field count"), Fields->GetNumFields(), 9);
	TestEqual(TEXT("Character link count"), Fields->GetNumLinks(), 2);

	TestEqual(TEXT("Changed event name is <Field>Changed"), UPsDataTestCharacter::GetHealthChangedEventName(), FString(TEXT("HealthChanged")));

	// DMETA lands on the field descriptor every instance shares
	const auto Character = NewObject<UPsDataTestCharacter>();
	TestEqual(TEXT("Alias renames the serialized key"), Character->MaxHealth.StaticField()->GetNameForSerialize(), FString(TEXT("hp_max")));
	TestTrue(TEXT("Strict meta parsed"), Character->Location.StaticField()->Meta.bStrict);
	TestTrue(TEXT("EventStorage meta parsed"), Character->Health.StaticField()->Meta.bEventStorage);
	TestFalse(TEXT("No Event meta on Kills"), Character->Kills.StaticField()->Meta.bEvent);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataPropertiesTest, "PsData.Properties",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataPropertiesTest::RunTest(const FString& Parameters)
{
	const auto Character = NewObject<UPsDataTestCharacter>();

	TestEqual(TEXT("InitProperties sets defaults"), *Character->Health, 100);
	TestEqual(TEXT("Unset int32 is zero"), *Character->Kills, 0);
	TestTrue(TEXT("Unset enum is its zero value"), Character->Team == EPsDataTestTeam::None);
	TestTrue(TEXT("Unset string is empty"), Character->Name->IsEmpty());

	TestNotNull(TEXT("Strict property is allocated"), *Character->Location);
	TestTrue(TEXT("Strict child is parented to its owner"), Character->Location->GetParent() == Character);

	Character->Name = TEXT("Alice");
	Character->Health -= 30;
	++Character->Kills;
	Character->Kills += 2;
	Character->Team = EPsDataTestTeam::Blue;
	Character->Location->X = 7;

	TestEqual(TEXT("FString assignment"), *Character->Name, FString(TEXT("Alice")));
	TestEqual(TEXT("Compound assignment"), *Character->Health, 70);
	TestEqual(TEXT("Increment and +="), *Character->Kills, 3);
	TestTrue(TEXT("Enum assignment"), Character->Team == EPsDataTestTeam::Blue);
	TestEqual(TEXT("Nested property"), *Character->Location->X, 7);

	Character->Reset();
	TestEqual(TEXT("Reset re-runs InitProperties"), *Character->Health, 100);
	TestTrue(TEXT("Reset clears the rest"), Character->Name->IsEmpty() && Character->Kills == 0 && Character->Location->X == 0);
	return true;
}

#endif
