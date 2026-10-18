// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsDataDefines.h"

#include "CoreMinimal.h"
#if PSDATA_ENGINE_AT_LEAST(5, 5)
#include "StructUtils/UserDefinedStruct.h"
#else
#include "Engine/UserDefinedStruct.h"
#endif

#include "PsDataStruct.generated.h"

class UPsData;

UCLASS()
class PSDATA_API UPsDataStruct : public UUserDefinedStruct
{
	GENERATED_BODY()

public:
	static FString GetStructName(UClass* PsDataClass);
	static UPsDataStruct* Find(UClass* PsDataClass);
	static UPsDataStruct* Create(UClass* PsDataClass, UPsData* CDO);

	UPsDataStruct();

	virtual FProperty* CustomFindProperty(const FName Name) const override;
	virtual void InitializeStruct(void* Dest, int32 ArrayDim = 1) const override;

protected:
	void Finalize(uint8* DefaultStruct);

	virtual void BeginDestroy() override;

private:
	uint8* RawStruct;
};
