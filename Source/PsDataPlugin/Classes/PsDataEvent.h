// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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

	UFUNCTION(BlueprintPure, Category = "PsData|Event")
	static UPsDataEvent* ConstructEvent(FString EventType, bool bEventBubbles);

private:
	friend class UPsData;
	friend class UPsDataEventFunctionLibrary;
	friend struct FCustomThunkTemplates_PsDataEvent;

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

	UFUNCTION(BlueprintPure, Category = "PsData|Event")
	const FString& GetType() const;

	UFUNCTION(BlueprintPure, Category = "PsData|Event")
	bool IsBubbles() const;

	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	void StopImmediatePropagation();

	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	void StopPropagation();
};

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsDataEvent"))
class PSDATAPLUGIN_API UPsDataEventFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/***********************************
	 * Event
	 ***********************************/

	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Event", meta = (DisplayName = "Get Target"))
	static void GetEventTarget(UPsDataEvent* Event, UPsData*& Target);

	DECLARE_FUNCTION(execGetEventTarget);
};

struct PSDATAPLUGIN_API FCustomThunkTemplates_PsDataEvent
{
public:
	static void GetEventTarget(UPsDataEvent* Event, UPsData*& Target);
};