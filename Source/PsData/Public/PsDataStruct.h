// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataDefines.h"

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"

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
