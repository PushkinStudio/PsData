// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Engine/DataTable.h"
#include "Misc/AutomationTest.h"
#include "PsDataStruct.h"
#include "Serialize/PsDataTableSerialization.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataTableImportTest, "PsData.DataTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataTableImportTest::RunTest(const FString& Parameters)
{
	// MAKE_TABLE_STRUCT generated a row struct when reflection compiled; it is what a designer picks in the editor
	const auto RowStruct = UPsDataStruct::Find(UPsDataTestWeaponProto::StaticClass());
	if (!TestNotNull(TEXT("Row struct generated"), RowStruct))
	{
		return false;
	}
	TestEqual(TEXT("Row struct name"), RowStruct->GetName(), FString(TEXT("PsDataTestWeaponProto_DataTableRow")));

	const auto Table = NewObject<UDataTable>();
	Table->RowStruct = RowStruct;
	const auto Problems = Table->CreateTableFromJSONString(TEXT(R"([
		{"Name": "rifle", "Damage": 30, "FireRate": "10.5"},
		{"Name": "shotgun", "Damage": 80, "FireRate": "1.25"}
	])"));
	TestEqual(TEXT("Table imports without problems"), Problems.Num(), 0);
	for (const auto& Problem : Problems)
	{
		AddError(Problem);
	}

	// Rows land in a DMAP keyed by row name
	const auto World = NewObject<UPsDataTestWorld>();
	FPsDataTableDeserializer Deserializer(Table, World->Weapons.Get());
	World->DataDeserialize(&Deserializer, true);

	TestEqual(TEXT("Every row became an element"), World->Weapons->Num(), 2);
	if (const auto Shotgun = World->Weapons->Find(TEXT("shotgun")))
	{
		TestEqual(TEXT("int32 column"), *(*Shotgun)->Damage, 80);
		TestTrue(TEXT("Fixed-point column"), (*Shotgun)->FireRate == FPsDataFixedPoint(TEXT("1.25")));
	}
	else
	{
		AddError(TEXT("Row 'shotgun' is missing"));
	}
	return true;
}

#endif
