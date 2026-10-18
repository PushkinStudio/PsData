// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

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
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	friend FArchive& operator<<(FArchive& Ar, FPsNetworkByteBuffer& Value);
};

/***********************************
 * EPsNetworkEventType
 ***********************************/

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
	FPsNetworkEvent(EPsNetworkEventType InType, const TArray<uint16>& InPath, const TArray<uint8>& InBuffer);
	FPsNetworkEvent(EPsNetworkEventType InType, const TArray<uint16>& InPath);

	EPsNetworkEventType Type;
	TArray<uint16> Path;
	FPsNetworkByteBuffer Data;

	bool Serialize(FArchive& Ar);
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	friend FArchive& operator<<(FArchive& Ar, FPsNetworkEvent& Value);
};

/***********************************
 * APsDataNetworkActor
 ***********************************/

UCLASS()
class PSDATA_API APsDataNetworkActor : public AActor
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
	APsDataNetworkActor();

	bool IsAuthority() const;

	bool IsConfirmed() const;

	bool IsSynchronized() const;

protected:
	friend class UPsNetworkData;

	void Open(UPsNetworkData* InParent);

	void Close();

	void Synchronize(const TArray<FString>& Dictionary, const FPsNetworkByteBuffer& Buffer);

	void Send(const TArray<FString>& Dictionary, const TArray<FPsNetworkEvent>& Events);

private:
	TArray<FPsNetworkByteBuffer> MakeDictionaryForReplication(const TArray<FString>& Dictionary);

	TArray<FString> MakeDictionaryFromReplication(const TArray<FPsNetworkByteBuffer>& Dictionary) const;

	UFUNCTION(Server, Reliable)
	void Server_Confirm();

	UFUNCTION(Client, Reliable)
	void Client_Synchronize(const TArray<FPsNetworkByteBuffer>& Dictionary, const FPsNetworkByteBuffer& Buffer);

	UFUNCTION(Client, Reliable)
	void Client_Send(const TArray<FPsNetworkByteBuffer>& Dictionary, const TArray<FPsNetworkEvent>& Events);

	EProxyState State;

	int32 LastDictionarySize;

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

	/** Accumulate events */
	bool bAccumulateEvents;

	void OpenConnection(APlayerController* Controller) const;

	void CloseConnection(APlayerController* Controller) const;

	bool HasAuthority() const;

	void FlushRequest() const;

	void Flush();

	FPsDataSimplePromise& OnSynchronizePromise() const;

private:
	friend class UPsData;
	friend class APsDataNetworkActor;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	void CommitChanges(const UPsData* Data, const FDataField* Field);

	void CommitAddedEvent(const UPsData* Data);

	void CommitRemovingEvent(const UPsData* Data);

	void HandlingControllers();

	void Synchronize(const TArray<FString>& DictionaryDiff, const FPsNetworkByteBuffer& Buffer);

	void Apply(const TArray<FString>& DictionaryDiff, const TArray<FPsNetworkEvent>& Events);

	bool ApplyChanged(FAbstractDataProperty* Property, const FPsNetworkByteBuffer& Buffer) const;

	bool ApplyAddedEvent(FAbstractDataProperty* Property, const FString& Key, const FPsNetworkByteBuffer& Buffer) const;

	bool ApplyRemovingEvent(FAbstractDataProperty* Property, const FString& Key) const;

	void MutableReset() const;

	TArray<FString> Dictionary;

	TArray<FPsNetworkEvent> NetworkEvents;

	float AccumulatedTime;

	mutable int32 NumAuthorityProxies;

	mutable bool bForceFlush;

	UPROPERTY()
	mutable TArray<APlayerController*> PendingControllers;

	UPROPERTY()
	mutable TArray<APsDataNetworkActor*> NetworkProxies;

	mutable FPsDataSimplePromise SynchronizePromise;
};

template <>
struct TStructOpsTypeTraits<FPsNetworkByteBuffer> : public TStructOpsTypeTraitsBase2<FPsNetworkByteBuffer>
{
	enum
	{
		WithZeroConstructor = true,
		WithSerializer = true,
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
	static constexpr EPropertyObjectReferenceType WithSerializerObjectReferences = EPropertyObjectReferenceType::None;
};

template <>
struct TStructOpsTypeTraits<FPsNetworkEvent> : public TStructOpsTypeTraitsBase2<FPsNetworkEvent>
{
	enum
	{
		WithZeroConstructor = true,
		WithSerializer = true,
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
	static constexpr EPropertyObjectReferenceType WithSerializerObjectReferences = EPropertyObjectReferenceType::None;
};