// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataMapProxy.h"

#include "CoreMinimal.h"

#include "PsDataBlueprintMapProxy.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UPsDataBlueprintMapProxy : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	FPsDataMapProxy<UPsData*> Proxy;

public:
	/** Initialize */
	void Init(UPsData* Instance, const TSharedPtr<const FDataField> Field);

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
	FPsDataConstMapProxy<UPsData*> GetProxy();

protected:
	/** Blueprint bind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind", Category = "PsData|Collection"))
	void BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

	/** Blueprint unbind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unbind", Category = "PsData|Collection"))
	void BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

	/** Blueprint get ref */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Map", Category = "PsData|Collection"))
	const TMap<FString, UPsData*>& GetRef();
};
