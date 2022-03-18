// Copyright 2021-2022 dawg.team. All Rights Reserved.

#pragma once

#include "PsData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tickable.h"

#include "PsNetworkData.generated.h"

class UPsNetworkData;
class APlayerController;

DEFINE_LOG_CATEGORY_STATIC(LogDataNetwork, VeryVerbose, All);

/***********************************
 * FPsNetworkByteBuffer
 ***********************************/

USTRUCT()
struct FPsNetworkByteBuffer
{
	GENERATED_BODY()

	FPsNetworkByteBuffer();
	FPsNetworkByteBuffer(const TArray<uint8>& InBuffer);

	TArray<uint8> Buffer;

	bool Serialize(FArchive& Ar);
	bool Serialize(FStructuredArchive::FSlot Slot);
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	friend FArchive& operator<<(FArchive& Ar, FPsNetworkByteBuffer& Value);
	friend void operator<<(FStructuredArchive::FSlot Slot, FPsNetworkByteBuffer& Value);
};

UENUM(BlueprintType, Blueprintable)
enum class EPsNetworkEventType : uint8
{
	None = 0,
	Changed = 1,
	Added = 2,
	Removed = 3,
};

/***********************************
 * FPsNetworkEvent
 ***********************************/

USTRUCT()
struct FPsNetworkEvent
{
	GENERATED_BODY()

	FPsNetworkEvent();
	FPsNetworkEvent(EPsNetworkEventType InType, const FString& InPath, const TArray<uint8>& InBuffer);

	EPsNetworkEventType Type;
	FString Path;
	FPsNetworkByteBuffer Data;

	bool Serialize(FArchive& Ar);
	bool Serialize(FStructuredArchive::FSlot Slot);
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	friend FArchive& operator<<(FArchive& Ar, FPsNetworkEvent& Value);
	friend void operator<<(FStructuredArchive::FSlot Slot, FPsNetworkEvent& Value);
};

/***********************************
 * FPsNetworkEventBundle
 ***********************************/

USTRUCT()
struct FPsNetworkEventBundle
{
	GENERATED_BODY()

	FPsNetworkEventBundle();

	void AddEvent(EPsNetworkEventType InType, const FString& InPath, const TArray<uint8>& InBuffer);

	TArray<FPsNetworkEvent> GetBundle() const;

	void Reset();

	bool HasEvents() const;

	bool Serialize(FArchive& Ar);
	bool Serialize(FStructuredArchive::FSlot Slot);
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	friend FArchive& operator<<(FArchive& Ar, FPsNetworkEventBundle& Value);
	friend void operator<<(FStructuredArchive::FSlot Slot, FPsNetworkEventBundle& Value);

private:
	TArray<FPsNetworkEvent> Events;
};

/***********************************
 * ADataNetworkActor
 ***********************************/

UCLASS()
class PSDATA_API ADataNetworkActor : public AActor
{
	GENERATED_BODY()

	enum class EProxyState
	{
		Created,
		Opened,
		Confirmed,
		Synchronized,
		Closed
	};

public:
	ADataNetworkActor();

	bool IsAuthority() const;

	bool IsConfirmed() const;

	bool IsSynchronized() const;

protected:
	friend class UPsNetworkData;

	void Open(UPsNetworkData* InParent);

	void Close();

	void Synchronize(const FPsNetworkByteBuffer& Buffer);

	void Send(const FPsNetworkEventBundle& Events);

private:
	UFUNCTION(Server, Reliable)
	void Server_Confirm();

	UFUNCTION(Client, Reliable)
	void Client_Synchronize(const FPsNetworkByteBuffer& Buffer);

	UFUNCTION(Client, Reliable)
	void Client_Send(const FPsNetworkEventBundle& Events);

	EProxyState State;

	UPROPERTY()
	UPsNetworkData* NetworkData;
};

/***********************************
 * UPsNetworkData
 ***********************************/

UCLASS()
class PSDATA_API UPsNetworkData : public UPsData, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPsNetworkData();

	/** How often (per second) this data will be considered for replication */
	float NetUpdateFrequency;

	void OpenConnection(APlayerController* Controller) const;

	void CloseConnection(APlayerController* Controller) const;

	bool HasAuthority() const;

	void FlushRequest() const;

	void Flush();

	FPsDataSimplePromise& OnSynchronizePromise() const;

private:
	friend class UPsData;
	friend class ADataNetworkActor;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	void CommitChanges(const UPsData* Data, const FDataField* Field);

	void CommitAddedEvent(const UPsData* Data);

	void CommitRemovingEvent(const UPsData* Data);

	void HandlingControllers();

	void Synchronize(const FPsNetworkByteBuffer& Buffer);

	void Apply(const FPsNetworkEventBundle& Events);

	bool ApplyChanged(FAbstractDataProperty* Property, const FPsNetworkByteBuffer& Buffer) const;

	bool ApplyAddedEvent(FAbstractDataProperty* Property, const FString& Key, const FPsNetworkByteBuffer& Buffer) const;

	bool ApplyRemovingEvent(FAbstractDataProperty* Property, const FString& Key) const;

	void MutableReset() const;

	FPsNetworkEventBundle NetworkEvents;

	float AccumulatedTime;

	mutable int32 NumAuthorityProxies;

	mutable bool bForceFlush;

	UPROPERTY()
	mutable TArray<APlayerController*> PendingControllers;

	UPROPERTY()
	mutable TArray<ADataNetworkActor*> NetworkProxies;

	mutable FPsDataSimplePromise SynchronizePromise;
};
