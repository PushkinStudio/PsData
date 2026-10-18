// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"
#include "Serialize/PsDataBinarySerialization.h"
#include "Serialize/PsDataFastJsonSerialization.h"
#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/Stream/PsDataBufferInputStream.h"
#include "Serialize/Stream/PsDataBufferOutputStream.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace PsDataJsonTypeTest
{
// Written with spaces and an exponent on purpose: what comes back is the canonical compact form
const TCHAR* Payload = TEXT(R"({"id": "a1", "count": 3, "price": 9.99, "sum": 0.30000000000000004, "ts": 1729270000123,
	"tags": ["x", "y"], "nested": {"ok": true, "none": null}, "neg": -2.5e-3})");
const TCHAR* CanonicalPayload = TEXT(R"({"id":"a1","count":3,"price":9.99,"sum":0.30000000000000004,"ts":1729270000123,"tags":["x","y"],"nested":{"ok":true,"none":null},"neg":-0.0025})");

// Not only objects: an array, a string, a number, a literal and null are all values
const TArray<FString> Items = {TEXT("[1,2,3]"), TEXT("\"text\""), TEXT("42"), TEXT("true"), TEXT("null")};

UPsDataTestMessage* MakeMessage()
{
	const auto Message = NewObject<UPsDataTestMessage>();
	Message->Payload = FPsDataJson(Payload);
	for (const auto& Item : Items)
	{
		Message->Items->Add(FPsDataJson(Item));
	}
	Message->Updates->Add(TEXT("alice"), FPsDataJson(TEXT("{\"hp\": 7}")));
	return Message;
}

void ExpectCanonical(FAutomationTestBase& Test, const FString& Format, const UPsDataTestMessage* Message)
{
	Test.TestEqual(Format + TEXT(": object payload"), Message->Payload->GetJsonString(), FString(CanonicalPayload));
	Test.TestEqual(Format + TEXT(": item count"), Message->Items->Num(), Items.Num());
	for (int32 i = 0; i < Items.Num() && i < Message->Items->Num(); ++i)
	{
		Test.TestEqual(Format + TEXT(": item ") + Items[i], Message->Items->Get(i).GetJsonString(), Items[i]);
	}

	const auto Update = Message->Updates->Find(TEXT("alice"));
	Test.TestTrue(Format + TEXT(": map value"), Update && Update->GetJsonString() == TEXT("{\"hp\":7}"));
}
} // namespace PsDataJsonTypeTest

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataJsonTypeTest, "PsData.Types.Json",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataJsonTypeTest::RunTest(const FString& Parameters)
{
	using namespace PsDataJsonTypeTest;

	TestTrue(TEXT("Default is null"), NewObject<UPsDataTestMessage>()->Payload->IsNull());

	const auto Source = MakeMessage();

	// Fast JSON: the payload is embedded as JSON, not as a string
	{
		FPsDataFastJsonSerializer Serializer;
		Source->DataSerialize(&Serializer);
		const FString Json = Serializer.GetJsonString();
		TestTrue(TEXT("Fast JSON embeds the value"), Json.Contains(TEXT("\"Payload\":{\"id\":\"a1\"")));

		const auto Target = NewObject<UPsDataTestMessage>();
		FPsDataFastJsonDeserializer Deserializer(Json);
		Target->DataDeserialize(&Deserializer);
		ExpectCanonical(*this, TEXT("Fast JSON"), Target);
	}

	// JSON DOM
	{
		FPsDataJsonSerializer Serializer;
		Source->DataSerialize(&Serializer);
		const auto Json = Serializer.GetJson();
		TestEqual(TEXT("JSON DOM embeds the value"), Json->GetObjectField(TEXT("Payload"))->GetArrayField(TEXT("tags")).Num(), 2);

		const auto Target = NewObject<UPsDataTestMessage>();
		FPsDataJsonDeserializer Deserializer(Json);
		Target->DataDeserialize(&Deserializer);
		ExpectCanonical(*this, TEXT("JSON DOM"), Target);
	}

	// Binary: int64 and double survive exactly
	{
		const auto Output = MakeShared<FPsDataBufferOutputStream>();
		FPsDataBinarySerializer Serializer(Output);
		Source->DataSerialize(&Serializer);

		const auto Target = NewObject<UPsDataTestMessage>();
		FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(Output->GetBuffer()));
		Target->DataDeserialize(&Deserializer);
		ExpectCanonical(*this, TEXT("Binary"), Target);
	}

	// Copy goes through the imprint path
	ExpectCanonical(*this, TEXT("Copy"), Source->Copy<UPsDataTestMessage>());
	TestEqual(TEXT("Equal payloads hash equal"), MakeMessage()->GetHash(), Source->GetHash());

	// A patch from a server
	{
		FPsDataFastJsonDeserializer Patch(TEXT("{\"Payload\": {\"x\": [1, 2.5, \"three\"]}}"));
		Source->DataDeserialize(&Patch, true);
		TestEqual(TEXT("Patch replaces the payload"), Source->Payload->GetJsonString(), FString(TEXT("{\"x\":[1,2.5,\"three\"]}")));
		TestEqual(TEXT("Patch keeps the rest"), Source->Items->Num(), Items.Num());
	}

	// Changes are compared as text
	int32 Changes = 0;
	FPsDataBindCollection Binds;
	Binds.Add(Source->Payload.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++Changes; })));
	Source->Payload = FPsDataJson(TEXT("{\"x\":[1,2.5,\"three\"]}"));
	TestEqual(TEXT("The same text is not a change"), Changes, 0);
	Source->Payload = FPsDataJson(TEXT("{\"X\":[1,2.5,\"three\"]}"));
	TestEqual(TEXT("Keys are case-sensitive"), Changes, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataTranslateTest, "PsData.Serialization.Translate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataTranslateTest::RunTest(const FString& Parameters)
{
	// A document converts between formats without the model: binary and JSON DOM to fast JSON
	const auto Root = PsDataTest::MakeModel();

	FPsDataFastJsonSerializer Direct;
	Root->DataSerialize(&Direct);
	const FString Expected = Direct.GetJsonString();

	{
		const auto Output = MakeShared<FPsDataBufferOutputStream>();
		FPsDataBinarySerializer Serializer(Output);
		Root->DataSerialize(&Serializer);

		FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(Output->GetBuffer()));
		FPsDataFastJsonSerializer Translated;
		TestTrue(TEXT("Binary translates"), Deserializer.Translate(&Translated));
		TestEqual(TEXT("Binary to fast JSON matches direct fast JSON"), Translated.GetJsonString(), Expected);
	}

	{
		FPsDataJsonSerializer Serializer;
		Root->DataSerialize(&Serializer);

		FPsDataJsonDeserializer Deserializer(Serializer.GetJson());
		FPsDataFastJsonSerializer Translated;
		TestTrue(TEXT("JSON DOM translates"), Deserializer.Translate(&Translated));
		TestEqual(TEXT("JSON DOM to fast JSON matches direct fast JSON"), Translated.GetJsonString(), Expected);
	}

	FPsDataFastJsonDeserializer Empty(TEXT(""));
	FPsDataFastJsonSerializer Nothing;
	TestFalse(TEXT("An empty document translates to nothing"), Empty.Translate(&Nothing));
	return true;
}

#endif
