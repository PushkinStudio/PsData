// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "Serialize/PsDataBinarySerialization.h"
#include "Serialize/Stream/PsDataInputStream.h"
#include "Serialize/Stream/PsDataOutputStream.h"

#include "CoreMinimal.h"

class UPsData;

/***********************************
 * FPsDataOptimizedBinarySerializer
 ***********************************/

struct PSDATA_API FPsDataOptimizedBinarySerializer : public FPsDataBinarySerializer
{
protected:
	TArray<FString>* KeyArray;

public:
	FPsDataOptimizedBinarySerializer(TSharedRef<FPsDataOutputStream> InOutputStream, TArray<FString>* InKeyArray);
	virtual ~FPsDataOptimizedBinarySerializer() {}

	virtual void WriteKey(const FString& Key) override;
};

/***********************************
 * FPsDataOptimizedBinaryDeserializer
 ***********************************/

struct PSDATA_API FPsDataOptimizedBinaryDeserializer : public FPsDataBinaryDeserializer
{
protected:
	const TArray<FString>* KeyArray;

public:
	FPsDataOptimizedBinaryDeserializer(TSharedRef<FPsDataInputStream> InInputStream, const TArray<FString>* InKeyArray);
	virtual ~FPsDataOptimizedBinaryDeserializer() {}

	virtual bool ReadKey(FString& OutKey) override;
};
