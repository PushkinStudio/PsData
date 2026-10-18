// Copyright 2015-2026 MY.GAMES. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataFixedPointTest, "PsData.Numbers.FixedPoint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataFixedPointTest::RunTest(const FString& Parameters)
{
	const FPsDataFixedPoint A(TEXT("1.25"));
	const FPsDataFixedPoint B(2);

	TestTrue(TEXT("Parsed from a string exactly"), A * FPsDataFixedPoint(4) == FPsDataFixedPoint(5));
	TestEqual(TEXT("Addition"), (A + B).ToFloat(), 3.25f);
	TestEqual(TEXT("Division"), (B / FPsDataFixedPoint(TEXT("0.5"))).ToInt(), static_cast<int64>(4));
	TestTrue(TEXT("Floor and Ceil"), A.Floor() == FPsDataFixedPoint(1) && A.Ceil() == FPsDataFixedPoint(2));
	TestTrue(TEXT("ToString parses back to the same value"), FPsDataFixedPoint(A.ToString()) == A);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPsDataBigIntegerTest, "PsData.Numbers.BigInteger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter)

bool FPsDataBigIntegerTest::RunTest(const FString& Parameters)
{
	FPsDataBigInteger Value(1);
	Value.ShiftLeft(100);
	TestEqual(TEXT("2^100"), Value.ToString(), FString(TEXT("1267650600228229401496703205376")));

	FPsDataBigInteger Product(TEXT("123456789012345678901234567890"));
	Product.Multiply(FPsDataBigInteger(TEXT("987654321098765432109876543210")));
	TestEqual(TEXT("Multiplication past 64 bits"), Product.ToString(), FString(TEXT("121932631137021795226185032733622923332237463801111263526900")));

	Product.Divide(FPsDataBigInteger(TEXT("987654321098765432109876543210")));
	TestEqual(TEXT("Division undoes it"), Product.ToString(), FString(TEXT("123456789012345678901234567890")));

	FPsDataBigInteger Negative(-5);
	Negative.Add(FPsDataBigInteger(3));
	TestEqual(TEXT("Signed addition"), Negative.ToInt64(), static_cast<int64>(-2));
	return true;
}

#endif
