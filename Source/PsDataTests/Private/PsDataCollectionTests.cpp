// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataTreeTest, "PsData.Tree",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataTreeTest::RunTest(const FString& Parameters)
{
	const auto Root = PsDataTest::MakeModel();
	const auto Alice = Root->Battle->Characters[TEXT("alice")];
	const auto Bob = Root->Battle->Characters[TEXT("bob")];

	TestEqual(TEXT("Data key is the map key"), Alice->GetDataKey(), FString(TEXT("alice")));
	TestEqual(TEXT("Collection key is the field name"), Alice->GetCollectionKey(), FString(TEXT("Characters")));
	TestEqual(TEXT("Full data key"), Alice->GetFullDataKey(), FString(TEXT("Characters.alice")));
	TestEqual(TEXT("Path from root"), Alice->GetPathFromRoot(), FString(TEXT("Battle.Characters.alice")));
	TestTrue(TEXT("Parent is the owner of the collection"), Alice->GetParent() == *Root->Battle);
	TestTrue(TEXT("Root is reachable from any depth"), Alice->Location->GetRoot() == Root);

	Root->Battle->Characters->Remove(TEXT("bob"));
	TestNull(TEXT("A removed element loses its parent"), Bob->GetParent());
	TestFalse(TEXT("A removed element loses its root"), Bob->HasRoot());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataCollectionsTest, "PsData.Collections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataCollectionsTest::RunTest(const FString& Parameters)
{
	const auto Root = PsDataTest::MakeModel();
	auto Characters = Root->Battle->Characters.Get();

	TestEqual(TEXT("Map Num"), Characters.Num(), 2);
	TestTrue(TEXT("Map Contains"), Characters.Contains(TEXT("bob")));
	TestNull(TEXT("Map Find on a missing key"), Characters.Find(TEXT("nobody")));
	const auto BlueOne = Characters.FindByPredicate([](const UPsDataTestCharacter* Character) {
		return Character->Team == EPsDataTestTeam::Blue;
	});
	TestTrue(TEXT("Map FindByPredicate"), BlueOne && (*BlueOne)->GetDataKey() == TEXT("bob"));

	TArray<FString> Keys;
	for (const auto& Pair : Root->Battle->Characters)
	{
		Keys.Add(Pair.Key);
		TestTrue(TEXT("Iterated value matches its key"), Pair.Value->GetDataKey() == Pair.Key);
	}
	Keys.Sort();
	TestTrue(TEXT("Map iteration visits every key"), Keys == TArray<FString>{TEXT("alice"), TEXT("bob")});

	const auto Alice = Characters[TEXT("alice")];
	Alice->Tags->Add(TEXT("medic"));
	TestEqual(TEXT("Array Num"), Alice->Tags->Num(), 2);
	TestEqual(TEXT("Array Get"), Alice->Tags->Get(1), FString(TEXT("medic")));
	Alice->Tags->RemoveAt(0);
	TestTrue(TEXT("Array RemoveAt"), Alice->Tags->Num() == 1 && Alice->Tags->Contains(TEXT("medic")));

	const auto Copy = Alice->Copy<UPsDataTestCharacter>();
	TestEqual(TEXT("Copy carries values"), *Copy->Name, FString(TEXT("Alice")));
	TestEqual(TEXT("Copy carries collections"), Copy->Tags->Num(), 1);
	TestNull(TEXT("Copy is detached from the tree"), Copy->GetParent());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataLinksTest, "PsData.Links",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataLinksTest::RunTest(const FString& Parameters)
{
	const auto Root = PsDataTest::MakeModel();
	const auto Alice = Root->Battle->Characters[TEXT("alice")];
	const auto Bob = Root->Battle->Characters[TEXT("bob")];

	TestTrue(TEXT("A valid model validates clean"), Root->Validation().Num() == 0);

	TestTrue(TEXT("Link resolves the key through the path"), Alice->LinkByWeapon.Get() == Root->World->Weapons[TEXT("rifle")]);
	TestEqual(TEXT("Linked data is readable"), *Alice->LinkByWeapon.Get()->Damage, 30);

	Alice->Weapon = TEXT("pistol");
	TestEqual(TEXT("Changing the key re-resolves the link"), *Alice->LinkByWeapon.Get()->Damage, 12);

	TestTrue(TEXT("Nullable link with an empty key is empty"), Bob->LinkByTarget.IsEmpty());
	TestNull(TEXT("Nullable link with an empty key is null"), Bob->LinkByTarget.Get());
	TestTrue(TEXT("Nullable link with a key resolves"), Alice->LinkByTarget.Get() == Bob);

	Root->Battle->Characters->Remove(TEXT("bob"));
	TestNull(TEXT("A link to removed data resolves to null"), Alice->LinkByTarget.Get());

	const auto Errors = Root->Validation();
	TestEqual(TEXT("Validation reports the dangling key"), Errors.Num(), 1);
	TestTrue(TEXT("Validation names the link and the key"), Errors.Num() == 1 && Errors[0].Contains(TEXT("Battle.Characters.alice.Target")) && Errors[0].Contains(TEXT("bob")));
	return true;
}

#endif
