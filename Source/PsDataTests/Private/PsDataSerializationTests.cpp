// Copyright 2015-2026 MY.GAMES. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"
#include "Serialize/PsDataBinarySerialization.h"
#include "Serialize/PsDataFastJsonSerialization.h"
#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/Stream/PsDataBufferInputStream.h"
#include "Serialize/Stream/PsDataBufferOutputStream.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataSerializationTest, "PsData.Serialization",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataSerializationTest::RunTest(const FString& Parameters)
{
	const auto Source = PsDataTest::MakeModel();
	const FString SourceHash = Source->GetHash();

	// FJsonObject: for tools and anything that already speaks the engine's JSON DOM
	{
		FPsDataJsonSerializer Serializer;
		Source->DataSerialize(&Serializer);
		const TSharedPtr<FJsonObject> Json = Serializer.GetJson();

		const auto AliceJson = Json->GetObjectField(TEXT("Battle"))->GetObjectField(TEXT("Characters"))->GetObjectField(TEXT("alice"));
		TestEqual(TEXT("JSON: alias is the key on the wire"), static_cast<int32>(AliceJson->GetNumberField(TEXT("hp_max"))), 100);
		TestEqual(TEXT("JSON: enum is written by name"), AliceJson->GetStringField(TEXT("Team")), FString(TEXT("Red")));

		const auto Target = NewObject<UPsDataTestRoot>();
		FPsDataJsonDeserializer Deserializer(Json);
		Target->DataDeserialize(&Deserializer);
		TestEqual(TEXT("JSON round trip keeps the content hash"), Target->GetHash(), SourceHash);
	}

	// Fast JSON: a string in, a string out, no DOM; what a save file or an HTTP body wants
	FString JsonString;
	{
		FPsDataFastJsonSerializer Serializer;
		Source->DataSerialize(&Serializer);
		JsonString = Serializer.GetJsonString();
		TestTrue(TEXT("Fast JSON: alias is the key on the wire"), JsonString.Contains(TEXT("\"hp_max\"")));

		const auto Target = NewObject<UPsDataTestRoot>();
		FPsDataFastJsonDeserializer Deserializer(JsonString);
		Target->DataDeserialize(&Deserializer);
		TestEqual(TEXT("Fast JSON round trip keeps the content hash"), Target->GetHash(), SourceHash);
	}

	// Binary: the compact form for the network and for snapshots
	{
		const auto OutputStream = MakeShared<FPsDataBufferOutputStream>();
		FPsDataBinarySerializer Serializer(OutputStream);
		Source->DataSerialize(&Serializer);
		AddInfo(FString::Printf(TEXT("Binary %d bytes, fast JSON %d chars"), OutputStream->GetBuffer().Num(), JsonString.Len()));

		const auto Target = NewObject<UPsDataTestRoot>();
		FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(OutputStream->GetBuffer()));
		Target->DataDeserialize(&Deserializer);
		TestEqual(TEXT("Binary round trip keeps the content hash"), Target->GetHash(), SourceHash);
		TestEqual(TEXT("Binary round trip keeps values"), *Target->Battle->Characters[TEXT("alice")]->Name, FString(TEXT("Alice")));
	}

	// Patch: apply a partial document on top of existing data
	{
		const auto Alice = Source->Battle->Characters[TEXT("alice")];

		FPsDataFastJsonDeserializer Patch(TEXT("{\"Health\":7}"));
		Alice->DataDeserialize(&Patch, true);
		TestEqual(TEXT("Patch writes the fields it names"), *Alice->Health, 7);
		TestEqual(TEXT("Patch keeps the rest"), *Alice->Name, FString(TEXT("Alice")));

		FPsDataFastJsonDeserializer Replace(TEXT("{\"Health\":8}"));
		Alice->DataDeserialize(&Replace);
		TestEqual(TEXT("Replace writes the fields it names"), *Alice->Health, 8);
		TestEqual(TEXT("Replace resets the rest"), *Alice->Name, FString());
		TestEqual(TEXT("Replace re-runs InitProperties"), *Alice->MaxHealth, 100);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataHashTest, "PsData.Hash",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataHashTest::RunTest(const FString& Parameters)
{
	const auto Root = PsDataTest::MakeModel();
	const auto Alice = Root->Battle->Characters[TEXT("alice")];
	const FString Before = Root->GetHash();

	TestEqual(TEXT("Same content, same hash"), PsDataTest::MakeModel()->GetHash(), Before);

	Alice->Location->Y = 3;
	TestNotEqual(TEXT("A change deep in the tree changes the root hash"), Root->GetHash(), Before);

	Alice->Location->Y = 0;
	TestEqual(TEXT("Restoring the content restores the hash"), Root->GetHash(), Before);
	return true;
}

#endif
