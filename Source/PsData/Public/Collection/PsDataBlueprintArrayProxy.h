// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataArrayProxy.h"
#include "Types/PsData_UPsData.h"

#include "CoreMinimal.h"

#include "PsDataBlueprintArrayProxy.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UPsDataBlueprintArrayProxy : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	FPsDataArrayProxy<UPsData*> Proxy;

public:
	/** Initialize */
	void Init(UPsData* Instance, const FDataField* Field);

	/** Is valid */
	UFUNCTION(BlueprintPure, meta = (Category = "PsData|Collection"))
	bool IsValid();

	/** Bind */
	void Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	void Bind(const FString& Type, const FPsDataDelegate& Delegate) const;

	/** Bind */
	void Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const;

	/** Get const proxy */
	FPsDataConstArrayProxy<UPsData*> GetProxy();

	/** Blueprint bind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind", Category = "PsData|Collection"))
	void BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

	/** Blueprint unbind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unbind", Category = "PsData|Collection"))
	void BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

protected:
	/** Blueprint get ref */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Map", Category = "PsData|Collection"))
	TArray<UPsData*> Get();
};
