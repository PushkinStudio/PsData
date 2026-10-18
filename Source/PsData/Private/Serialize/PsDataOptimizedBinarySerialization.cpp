// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Serialize/PsDataOptimizedBinarySerialization.h"

#include "PsData.h"

/***********************************
 * FPsDataOptimizedBinarySerializer
 ***********************************/

FPsDataOptimizedBinarySerializer::FPsDataOptimizedBinarySerializer(TSharedRef<FPsDataOutputStream> InOutputStream, TArray<FString>* InKeyArray)
	: FPsDataBinarySerializer(InOutputStream)
	, KeyArray(InKeyArray)
{
	check(KeyArray);
}

void FPsDataOptimizedBinarySerializer::WriteKey(const FString& Key)
{
	OutputStream->WriteUint8(EBinaryTokens::KeyBegin);

	auto Index = KeyArray->Find(Key);
	if (Index == INDEX_NONE)
	{
		Index = KeyArray->Add(Key);
	}

	check(TNumericLimits<uint16>::Max() >= Index);
	OutputStream->WriteUint16(Index);
}

/***********************************
 * FPsDataOptimizedBinaryDeserializer
 ***********************************/

FPsDataOptimizedBinaryDeserializer::FPsDataOptimizedBinaryDeserializer(TSharedRef<FPsDataInputStream> InInputStream, const TArray<FString>* InKeyArray)
	: FPsDataBinaryDeserializer(InInputStream)
	, KeyArray(InKeyArray)
{
	check(KeyArray);
}

bool FPsDataOptimizedBinaryDeserializer::ReadKey(FString& OutKey)
{
	if (CheckToken(EBinaryTokens::KeyBegin))
	{
		const auto Index = InputStream->ReadUint16();
		check(KeyArray->IsValidIndex(Index));
		OutKey = KeyArray->GetData()[Index];
		return true;
	}
	return false;
}