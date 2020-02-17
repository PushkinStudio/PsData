// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PsDataEvent.generated.h"

class UPsData;
struct FDataField;

UCLASS(BlueprintType, Blueprintable)
class PSDATAPLUGIN_API UPsDataEvent : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Added */
	static const FString Added;

	/** Removing */
	static const FString Removing;

	/** Changed */
	static const FString Changed;

	/** Name Changed */
	static const FString NameChanged;

	/** Dispatch property changed */
	static void DispatchChange(UPsData* Instance, TSharedPtr<const FDataField> Field);

	UFUNCTION(BlueprintPure, Category = "PsData|Event")
	static UPsDataEvent* ConstructEvent(FString EventType, bool bEventBubbles);

private:
	friend class UPsData;

protected:
	UPROPERTY()
	FString Type;

	UPROPERTY()
	UPsData* Target;

	UPROPERTY()
	bool bBubbles;

	UPROPERTY()
	bool bStopImmediate;

	UPROPERTY()
	bool bStop;

public:
	/* Const target for c++ */
	const UPsData* GetTarget() const;

	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	const FString& GetType() const;

	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	bool IsBubbles() const;

	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	void StopImmediatePropagation();

	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	void StopPropagation();

protected:
	/* Mutable target for blueprint */
	UFUNCTION(BlueprintCallable, Category = "PsData|Event", meta = (DisplayName = "Get Target"))
	UPsData* GetTarget_Mutable() const;
};
