// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsDataEvent.generated.h"

using FPsDataEventScopeGuardCallback = TFunction<void()>;

struct PSDATA_API FPsDataEventScopeGuard
{
public:
	FPsDataEventScopeGuard();
	~FPsDataEventScopeGuard();

private:
	void Invoke();

public:
	static void AddCallback(FPsDataEventScopeGuardCallback Function);
	static bool IsGuarded();

private:
	static int32 Index;
	static TArray<FPsDataEventScopeGuardCallback> Callbacks;
};

class UPsData;

UCLASS(BlueprintType, Blueprintable)
class PSDATA_API UPsDataEvent : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** Added */
	static const FString Added;

	/** Added To Parent */
	static const FString AddedToParent;

	/** Added To Root */
	static const FString AddedToRoot;

	/** Removed */
	static const FString Removed;

	/** Removed From Parent */
	static const FString RemovedFromParent;

	/** Removed From Root */
	static const FString RemovedFromRoot;

	/** Changed */
	static const FString Changed;

	/** Name Changed */
	static const FString NameChanged;

	UFUNCTION(BlueprintPure, Category = "PsData|Event")
	static UPsDataEvent* ConstructEvent(FString EventType, bool bEventBubbles, UClass* EventClass = nullptr);

	template <class EventClass>
	static EventClass* ConstructEvent(FString EventType, bool bEventBubbles)
	{
		return CastChecked<EventClass>(ConstructEvent(EventType, bEventBubbles, EventClass::StaticClass()));
	}

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
class PSDATA_API UPsDataEventFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Event", meta = (DisplayName = "Get Target"))
	static void GetEventTarget(UPsDataEvent* Event, UPsData*& Target);

	DECLARE_FUNCTION(execGetEventTarget);
};

struct PSDATA_API FCustomThunkTemplates_PsDataEvent
{
public:
	static void GetEventTarget(UPsDataEvent* Event, UPsData*& Target);
};