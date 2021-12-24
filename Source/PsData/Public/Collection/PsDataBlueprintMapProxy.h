// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataMapProxy.h"
#include "Types/PsData_UPsData.h"

#include "CoreMinimal.h"

#include "PsDataBlueprintMapProxy.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UPsDataBlueprintMapProxy : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	PsDataTools::TDataProperty<TMap<FString, UPsData*>>* Property;

public:
	/** Initialize */
	void Init(PsDataTools::TDataProperty<TMap<FString, UPsData*>>* InProperty);

	/** Is valid */
	UFUNCTION(BlueprintPure, meta = (Category = "PsData|Collection"))
	bool IsValid();

	/** Blueprint bind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind", Category = "PsData|Collection"))
	void BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

	/** Blueprint unbind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unbind", Category = "PsData|Collection"))
	void BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

protected:
	/** Blueprint get */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Map", Category = "PsData|Collection"))
	TMap<FString, UPsData*> Get();
};
