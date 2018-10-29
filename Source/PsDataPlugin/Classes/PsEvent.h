// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsEvent.generated.h"

class UPsData;

UCLASS(BlueprintType, Blueprintable)
class PSDATAPLUGIN_API UPsEvent : public UObject
{
	GENERATED_UCLASS_BODY()
	
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
	UFUNCTION(BlueprintPure, Category = "PsData|Event")
	static UPsEvent* ConstructEvent(FString EventType, bool bEventBubbles);
	
	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	FString GetType() const;
	
	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	UPsData* GetTarget() const;
	
	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	bool IsBubbles() const;
	
	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	void StopImmediatePropagation();
	
	UFUNCTION(BlueprintCallable, Category = "PsData|Event")
	void StopPropagation();
	
};
