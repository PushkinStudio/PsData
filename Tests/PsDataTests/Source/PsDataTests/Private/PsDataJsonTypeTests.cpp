// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"
#include "Serialization/JsonSerializer.h"
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataJsonTypeTest, "PsData.Types.Json.Formats",
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataJsonCanonicalTest, "PsData.Types.Json.Canonical",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataJsonCanonicalTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Stored in compact form"), FPsDataJson(TEXT("{ \"hp\" : 7 }")).GetJsonString(), FString(TEXT("{\"hp\":7}")));
	TestTrue(TEXT("Blank text is null"), FPsDataJson(TEXT("  \n")).IsNull());

	// Text that is not JSON never reaches a document: it is refused when stored
	AddExpectedError(TEXT("not valid JSON"), EAutomationExpectedErrorFlags::Contains, 1);
	const auto Message = NewObject<UPsDataTestMessage>();
	Message->Payload = FPsDataJson(TEXT("{\"a\": }"));
	TestTrue(TEXT("Invalid JSON is stored as null"), Message->Payload->IsNull());

	Message->Items->Add(FPsDataJson(TEXT("[1, 2]")));
	FPsDataFastJsonSerializer Serializer;
	Message->DataSerialize(&Serializer);
	TSharedPtr<FJsonObject> Parsed;
	TestTrue(TEXT("The document stays valid JSON"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Serializer.GetJsonString()), Parsed) && Parsed.IsValid());

	// A server sending the same value with other spacing is not a change
	Message->Payload = FPsDataJson(TEXT("{\"hp\": 7}"));
	int32 Changes = 0;
	FPsDataBindCollection Binds;
	Binds.Add(Message->Payload.Bind(FPsDataDelegate::CreateLambda([&](UPsDataEvent*) { ++Changes; })));
	FPsDataFastJsonDeserializer Patch(TEXT("{\"Payload\": {\"hp\":7}}"));
	Message->DataDeserialize(&Patch, true);
	TestEqual(TEXT("Same value, other spacing: no event"), Changes, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataEscapesTest, "PsData.Serialization.Escapes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataEscapesTest::RunTest(const FString& Parameters)
{
	// What other JSON writers produce: \u escapes, an escaped slash, short forms
	{
		const auto Character = NewObject<UPsDataTestCharacter>();
		FPsDataFastJsonDeserializer Deserializer(TEXT(R"({"Name": "café \/ A\tB"})"));
		Character->DataDeserialize(&Deserializer, true);
		TestEqual(TEXT("Escapes from other writers are read"), *Character->Name, FString(TEXT("café / A\tB")));
	}

	// Control characters, quotes and backslashes survive a round trip and leave valid JSON behind
	{
		const FString Name = TEXT("line\nnext\x01 \"quoted\" back\\slash é");
		const auto Character = NewObject<UPsDataTestCharacter>();
		Character->Name = Name;

		FPsDataFastJsonSerializer Serializer;
		Character->DataSerialize(&Serializer);
		const FString Json = Serializer.GetJsonString();
		TestTrue(TEXT("A control character is written as \\u00XX"), Json.Contains(TEXT("\\u0001")));

		TSharedPtr<FJsonObject> Parsed;
		TestTrue(TEXT("The engine's parser accepts the output"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Parsed) && Parsed.IsValid());

		const auto Target = NewObject<UPsDataTestCharacter>();
		FPsDataFastJsonDeserializer Deserializer(Json);
		Target->DataDeserialize(&Deserializer);
		TestEqual(TEXT("The string comes back unchanged"), *Target->Name, Name);
	}

	// Keys are escaped too: in a free-form payload they are arbitrary text
	{
		const FPsDataJson Payload(TEXT(R"({"café": "a\"b\\c\/d\u0001\b\f\n", "k\"ey": 1})"));
		const FString Expected = TEXT("{\"café\":\"a\\\"b\\\\c/d\\u0001\\b\\f\\n\",\"k\\\"ey\":1}");
		TestEqual(TEXT("Keys and values in canonical form"), Payload.GetJsonString(), Expected);

		const auto Message = NewObject<UPsDataTestMessage>();
		Message->Payload = Payload;
		FPsDataFastJsonSerializer Serializer;
		Message->DataSerialize(&Serializer);
		const auto Target = NewObject<UPsDataTestMessage>();
		FPsDataFastJsonDeserializer Deserializer(Serializer.GetJsonString());
		Target->DataDeserialize(&Deserializer);
		TestEqual(TEXT("A payload with escaped keys round-trips"), Target->Payload->GetJsonString(), Expected);
	}

	// Every int64 survives fast JSON and binary; past the int64 range a number is a double
	{
		const FString Ids = TEXT("{\"id\":1234567890123456789,\"max\":9223372036854775807,\"neg\":-9223372036854775807,\"big\":1.2345678901234567e+19}");
		const auto Message = NewObject<UPsDataTestMessage>();
		Message->Payload = FPsDataJson(TEXT("{\"id\": 1234567890123456789, \"max\": 9223372036854775807, \"neg\": -9223372036854775807, \"big\": 12345678901234567890}"));
		TestEqual(TEXT("19-digit integers are kept exactly"), Message->Payload->GetJsonString(), Ids);

		const auto Output = MakeShared<FPsDataBufferOutputStream>();
		FPsDataBinarySerializer Serializer(Output);
		Message->DataSerialize(&Serializer);
		const auto Target = NewObject<UPsDataTestMessage>();
		FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(Output->GetBuffer()));
		Target->DataDeserialize(&Deserializer);
		TestEqual(TEXT("19-digit integers survive binary"), Target->Payload->GetJsonString(), Ids);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataTruncatedTest, "PsData.Serialization.Truncated",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataTruncatedTest::RunTest(const FString& Parameters)
{
	// A cut-off server response must not take the game down: these used to read past the token list
	const TArray<FString> Broken = {TEXT("{\"a\":"), TEXT("[1, 2"), TEXT("{\"a\": [1, {\"b\""), TEXT("{")};
	AddExpectedError(TEXT("not valid JSON"), EAutomationExpectedErrorFlags::Contains, Broken.Num());
	for (const auto& Text : Broken)
	{
		TestTrue(TEXT("Truncated payload is null: ") + Text, FPsDataJson(Text).IsNull());
	}

	const auto Character = NewObject<UPsDataTestCharacter>();
	FPsDataFastJsonDeserializer Truncated(TEXT("{\"Name\": \"Alice\", \"Tags\": [\"a\", \"b\""));
	Character->DataDeserialize(&Truncated, true);
	TestEqual(TEXT("What arrived before the cut is applied"), *Character->Name, FString(TEXT("Alice")));

	FPsDataFastJsonDeserializer Empty(TEXT(""));
	Character->DataDeserialize(&Empty, true);
	TestEqual(TEXT("An empty document changes nothing"), *Character->Name, FString(TEXT("Alice")));
	return true;
}

#endif
