// Copyright 2021-2022 dawg.team. All Rights Reserved.

#include "PsNetworkData.h"

#include "PsDataAPI.h"

#include "Engine/Engine.h"
#include "Engine/NetDriver.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

#include <string>

using namespace PsDataTools;

/***********************************
 * FPsNetworkByteBuffer
 ***********************************/

FPsNetworkByteBuffer::FPsNetworkByteBuffer()
{
}

FPsNetworkByteBuffer::FPsNetworkByteBuffer(const TArray<uint8>& InBuffer)
	: Buffer(InBuffer)
{
}

bool FPsNetworkByteBuffer::Serialize(FArchive& Ar)
{
	Ar << *this;
	return true;
}

bool FPsNetworkByteBuffer::Serialize(FStructuredArchive::FSlot Slot)
{
	Slot << *this;
	return true;
}

bool FPsNetworkByteBuffer::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << *this;

	bOutSuccess = true;
	return true;
}

FArchive& operator<<(FArchive& Ar, FPsNetworkByteBuffer& Value)
{
	return Ar << Value.Buffer;
}

void operator<<(FStructuredArchive::FSlot Slot, FPsNetworkByteBuffer& Value)
{
	Slot << Value.Buffer;
}

template <>
struct TStructOpsTypeTraits<FPsNetworkByteBuffer> : public TStructOpsTypeTraitsBase2<FPsNetworkByteBuffer>
{
	enum
	{
		WithZeroConstructor = true,
		WithSerializer = true,
		WithStructuredSerializer = true,
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
};
IMPLEMENT_STRUCT(PsNetworkByteBuffer);

/***********************************
 * FPsNetworkEvent
 ***********************************/

FPsNetworkEvent::FPsNetworkEvent()
	: Type(EPsNetworkEventType::None)
{
}

FPsNetworkEvent::FPsNetworkEvent(EPsNetworkEventType InType, const FString& InPath, const TArray<uint8>& InBuffer)
	: Type(InType)
	, Path(InPath)
	, Data(InBuffer)
{
}

bool FPsNetworkEvent::Serialize(FArchive& Ar)
{
	Ar << *this;
	return true;
}

bool FPsNetworkEvent::Serialize(FStructuredArchive::FSlot Slot)
{
	Slot << *this;
	return true;
}

bool FPsNetworkEvent::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << *this;

	bOutSuccess = true;
	return true;
}

FArchive& operator<<(FArchive& Ar, FPsNetworkEvent& Value)
{
	Ar << Value.Type;

	if (Ar.IsLoading())
	{
		TArray<char> CharKey;
		Ar << CharKey;

		const auto Converter = FUTF8ToTCHAR(CharKey.GetData(), CharKey.Num());
		Value.Path = FString(Converter.Length(), Converter.Get());
	}
	else if (Ar.IsSaving())
	{
		const auto Converter = FTCHARToUTF8(*Value.Path, Value.Path.Len());
		TArray<char> CharKey(Converter.Get(), Converter.Length());
		Ar << CharKey;
	}
	else
	{
		checkNoEntry();
	}

	const bool bHasData = Value.Type != EPsNetworkEventType::Removed;
	if (bHasData)
	{
		Ar << Value.Data;
	}

	return Ar;
}

void operator<<(FStructuredArchive::FSlot Slot, FPsNetworkEvent& Value)
{
	Slot << Value.Type;
	Slot << Value.Path;
	Slot << Value.Data;
}

template <>
struct TStructOpsTypeTraits<FPsNetworkEvent> : public TStructOpsTypeTraitsBase2<FPsNetworkEvent>
{
	enum
	{
		WithZeroConstructor = true,
		WithSerializer = true,
		WithStructuredSerializer = true,
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
};
IMPLEMENT_STRUCT(PsNetworkEvent);

/***********************************
 * FPsNetworkEventBundle
 ***********************************/

FPsNetworkEventBundle::FPsNetworkEventBundle()
{
}

void FPsNetworkEventBundle::AddEvent(EPsNetworkEventType InType, const FString& InPath, const TArray<uint8>& InBuffer)
{
	check(InType != EPsNetworkEventType::None);
	Events.Emplace(InType, InPath, InBuffer);
}

TArray<FPsNetworkEvent> FPsNetworkEventBundle::GetBundle() const
{
	return Events;
}

void FPsNetworkEventBundle::Reset()
{
	Events.Reset();
}

bool FPsNetworkEventBundle::HasEvents() const
{
	return Events.Num() > 0;
}

bool FPsNetworkEventBundle::Serialize(FArchive& Ar)
{
	Ar << *this;
	return true;
}

bool FPsNetworkEventBundle::Serialize(FStructuredArchive::FSlot Slot)
{
	Slot << *this;
	return true;
}

bool FPsNetworkEventBundle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << *this;

	bOutSuccess = true;
	return true;
}

FArchive& operator<<(FArchive& Ar, FPsNetworkEventBundle& Value)
{
	return Ar << Value.Events;
}

void operator<<(FStructuredArchive::FSlot Slot, FPsNetworkEventBundle& Value)
{
	Slot << Value.Events;
}

template <>
struct TStructOpsTypeTraits<FPsNetworkEventBundle> : public TStructOpsTypeTraitsBase2<FPsNetworkEventBundle>
{
	enum
	{
		WithZeroConstructor = true,
		WithSerializer = true,
		WithStructuredSerializer = true,
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
};
IMPLEMENT_STRUCT(PsNetworkEventBundle);

/***********************************
 * ADataNetworkActor
 ***********************************/

ADataNetworkActor::ADataNetworkActor()
	: State(EProxyState::Created)
	, NetworkData(nullptr)
{
	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
}

bool ADataNetworkActor::IsAuthority() const
{
	if (const auto Controller = Cast<APlayerController>(GetOwner()))
	{
		return Controller->HasAuthority();
	}

	return false;
}

bool ADataNetworkActor::IsConfirmed() const
{
	return State == EProxyState::Confirmed;
}

bool ADataNetworkActor::IsSynchronized() const
{
	return State == EProxyState::Synchronized;
}

void ADataNetworkActor::Open(UPsNetworkData* InNetworkData)
{
	NetworkData = InNetworkData;

	if (State == EProxyState::Closed)
	{
		return;
	}

	if (IsAuthority())
	{
		UE_LOG(LogDataNetwork, Display, TEXT("Server proxy opened"));
		State = EProxyState::Opened;
	}
	else
	{
		UE_LOG(LogDataNetwork, Display, TEXT("Client proxy opened"));
		State = EProxyState::Confirmed;
		Server_Confirm();
	}
}

void ADataNetworkActor::Close()
{
	NetworkData = nullptr;

	if (State == EProxyState::Closed)
	{
		return;
	}

	if (IsAuthority())
	{
		UE_LOG(LogDataNetwork, Display, TEXT("Server proxy closed"));
	}
	else
	{
		UE_LOG(LogDataNetwork, Display, TEXT("Client proxy closed"));
	}

	State = EProxyState::Closed;
	Destroy();
}

void ADataNetworkActor::Synchronize(const FPsNetworkByteBuffer& Buffer)
{
	if (State == EProxyState::Closed)
	{
		return;
	}

	check(IsAuthority() && State == EProxyState::Confirmed);
	State = EProxyState::Synchronized;
	Client_Synchronize(Buffer);
}

void ADataNetworkActor::Send(const FPsNetworkEventBundle& Events)
{
	if (State == EProxyState::Closed)
	{
		return;
	}

	check(IsAuthority() && State == EProxyState::Synchronized);
	Client_Send(Events);
}

void ADataNetworkActor::Server_Confirm_Implementation()
{
	check(IsAuthority());
	UE_LOG(LogDataNetwork, Display, TEXT("Server proxy confirmed"));
	State = EProxyState::Confirmed;
	NetworkData->FlushRequest();
	NetworkData->SynchronizePromise.Resolve();
}

void ADataNetworkActor::Client_Synchronize_Implementation(const FPsNetworkByteBuffer& Buffer)
{
	State = EProxyState::Synchronized;
	NetworkData->Synchronize(Buffer);
}

void ADataNetworkActor::Client_Send_Implementation(const FPsNetworkEventBundle& Events)
{
	check(State == EProxyState::Synchronized);
	NetworkData->Apply(Events);
}

/***********************************
 * UPsNetworkData
 ***********************************/

UPsNetworkData::UPsNetworkData()
	: NetUpdateFrequency(30.f)
	, AccumulatedTime(0.f)
	, NumAuthorityProxies(0)
	, bForceFlush(false)
{
}

void UPsNetworkData::OpenConnection(APlayerController* Controller) const
{
	for (const auto Proxy : NetworkProxies)
	{
		if (Proxy->GetOwner() == Controller)
		{
			UE_LOG(LogDataNetwork, Fatal, TEXT("Attempt to add an already existing proxy"));
		}
	}

	for (const auto PendingController : PendingControllers)
	{
		if (PendingController == Controller)
		{
			UE_LOG(LogDataNetwork, Fatal, TEXT("Attempt to add an already existing proxy"));
		}
	}

	const auto bAuthority = Controller->HasAuthority();
	if (bAuthority)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = Controller;
		SpawnParameters.Name = *FString::Printf(TEXT("DataNetworkActor_%s"), *Controller->GetName());
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const auto Proxy = Controller->GetWorld()->SpawnActor<ADataNetworkActor>(SpawnParameters);
		NetworkProxies.Add(Proxy);
		Proxy->Open(const_cast<UPsNetworkData*>(this));

		++NumAuthorityProxies;
		FlushRequest();
	}
	else
	{
		MutableReset();
		PendingControllers.Add(Controller);
	}
}

void UPsNetworkData::CloseConnection(APlayerController* Controller) const
{
	const auto bAuthority = Controller->HasAuthority();
	if (bAuthority)
	{
		--NumAuthorityProxies;
	}
	else
	{
		MutableReset();
	}

	for (auto It = NetworkProxies.CreateIterator(); It; ++It)
	{
		const auto Proxy = *It;
		if (Proxy->GetOwner() == Controller)
		{
			Proxy->Close();
			It.RemoveCurrent();
			return;
		}
	}

	for (auto It = PendingControllers.CreateIterator(); It; ++It)
	{
		const auto PendingController = *It;
		if (PendingController == Controller)
		{
			It.RemoveCurrent();
			return;
		}
	}

	UE_LOG(LogDataNetwork, Fatal, TEXT("Attempt to remove a non-existing proxy"));
}

bool UPsNetworkData::HasAuthority() const
{
	return NumAuthorityProxies > 0;
}

void UPsNetworkData::FlushRequest() const
{
	bForceFlush = true;
}

void UPsNetworkData::Flush()
{
	bForceFlush = false;
	if (HasAuthority())
	{
		TArray<ADataNetworkActor*> ConfirmedProxies;
		TArray<ADataNetworkActor*> SynchronizedProxies;

		for (const auto NetworkProxy : NetworkProxies)
		{
			if (NetworkProxy->IsAuthority())
			{
				if (NetworkProxy->IsConfirmed())
				{
					ConfirmedProxies.Add(NetworkProxy);
				}

				if (NetworkEvents.HasEvents())
				{
					if (NetworkProxy->IsSynchronized())
					{
						SynchronizedProxies.Add(NetworkProxy);
					}
				}
			}
		}

		const auto OutputBuffer = MakeShared<FPsDataBufferOutputStream>();
		if (SynchronizedProxies.Num() > 0)
		{
			const FPsNetworkByteBuffer Buffer(OutputBuffer->GetBuffer());
			for (const auto NetworkObject : SynchronizedProxies)
			{
				NetworkObject->Send(NetworkEvents);
			}
		}

		if (ConfirmedProxies.Num() > 0)
		{
			OutputBuffer->Reset();
			FPsDataBinarySerializer Serializer(OutputBuffer);
			Serializer.bWriteDefaults = false;
			DataSerialize(&Serializer);

			const FPsNetworkByteBuffer Buffer(OutputBuffer->GetBuffer());
			for (const auto NetworkObject : ConfirmedProxies)
			{
				NetworkObject->Synchronize(Buffer);
			}
		}
	}

	NetworkEvents.Reset();
}

FPsDataSimplePromise& UPsNetworkData::OnSynchronizePromise() const
{
	return SynchronizePromise;
}

void UPsNetworkData::Tick(float DeltaTime)
{
	if (HasAuthority())
	{
		auto MinDeltaTime = 0.f;
		if (NetUpdateFrequency > SMALL_NUMBER)
		{
			MinDeltaTime = 1.f / NetUpdateFrequency;
		}

		AccumulatedTime += DeltaTime;
		if (bForceFlush || AccumulatedTime >= MinDeltaTime)
		{
			Flush();
			AccumulatedTime = 0.f;
		}
	}

	HandlingControllers();
}

TStatId UPsNetworkData::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UPsNetworkData, STATGROUP_Tickables);
}

void UPsNetworkData::CommitChanges(const UPsData* Data, const FDataField* Field)
{
	if (!Field->Context->IsData())
	{
		const auto OutputBuffer = MakeShared<FPsDataBufferOutputStream>();
		FPsDataBinarySerializer Serializer(OutputBuffer);
		Serializer.bWriteDefaults = false;
		const auto Property = FPsDataFriend::GetProperty(Data, Field->Index);
		Property->Serialize(&Serializer);

		FString Path = Data->GetPathFromData(this);
		Path.AppendChar('.');
		Path.Append(Field->Name);

		NetworkEvents.AddEvent(EPsNetworkEventType::Changed, Path, OutputBuffer->GetBuffer());
	}
}

void UPsNetworkData::CommitAddedEvent(const UPsData* Data)
{
	const auto OutputBuffer = MakeShared<FPsDataBufferOutputStream>();
	FPsDataBinarySerializer Serializer(OutputBuffer);
	Serializer.bWriteDefaults = false;
	Data->DataSerialize(&Serializer);

	NetworkEvents.AddEvent(EPsNetworkEventType::Added, Data->GetPathFromData(this), OutputBuffer->GetBuffer());
}

void UPsNetworkData::CommitRemovingEvent(const UPsData* Data)
{
	NetworkEvents.AddEvent(EPsNetworkEventType::Removed, Data->GetPathFromData(this), {});
}

void UPsNetworkData::HandlingControllers()
{
	if (PendingControllers.Num() > 0)
	{
		for (auto ControllerIterator = PendingControllers.CreateIterator(); ControllerIterator; ++ControllerIterator)
		{
			const auto PendingController = *ControllerIterator;
			if (PendingController == nullptr)
			{
				ControllerIterator.RemoveCurrent();
			}
			else
			{
				for (TActorIterator<ADataNetworkActor> ActorIterator(PendingController->GetWorld()); ActorIterator; ++ActorIterator)
				{
					const auto PendingActor = *ActorIterator;
					if (PendingActor->GetOwner() == PendingController)
					{
						ControllerIterator.RemoveCurrent();
						PendingActor->Open(this);
						NetworkProxies.Add(PendingActor);
					}
				}
			}
		}
	}
}

void UPsNetworkData::Synchronize(const FPsNetworkByteBuffer& Buffer)
{
	check(!HasAuthority());

	DEFERRED_EVENT_PROCESSING();
	const auto InputBuffer = MakeShared<FPsDataBufferInputStream>(Buffer.Buffer);
	FPsDataBinaryDeserializer Deserializer(InputBuffer);
	DataDeserialize(&Deserializer, false);

	UE_LOG(LogDataNetwork, Display, TEXT("Client proxy synchronized"));
	SynchronizePromise.Resolve();
}

void UPsNetworkData::Apply(const FPsNetworkEventBundle& Events)
{
	check(!HasAuthority());

	DEFERRED_EVENT_PROCESSING();

	const auto EventsList = Events.GetBundle();
	for (const auto& Event : EventsList)
	{
		TDataPathExecutor<true, true> PathExecutor(this, Event.Path);

		FAbstractDataProperty* Property;
		if (PathExecutor.Execute(Property))
		{
			if (Event.Type == EPsNetworkEventType::Changed)
			{
				const bool bSuccess = ApplyChanged(Property, Event.Data);
				check(bSuccess);
			}
			else if (Event.Type == EPsNetworkEventType::Added)
			{
				const bool bSuccess = ApplyAddedEvent(Property, PathExecutor.GetPath(), Event.Data);
				check(bSuccess);
			}
			else if (Event.Type == EPsNetworkEventType::Removed)
			{
				const bool bSuccess = ApplyRemovingEvent(Property, PathExecutor.GetPath());
				check(bSuccess);
			}
		}
	}
}

bool UPsNetworkData::ApplyChanged(FAbstractDataProperty* Property, const FPsNetworkByteBuffer& Buffer) const
{
	FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(Buffer.Buffer));
	Property->Deserialize(&Deserializer);
	return true;
}

bool UPsNetworkData::ApplyAddedEvent(FAbstractDataProperty* Property, const FString& Key, const FPsNetworkByteBuffer& Buffer) const
{
	const auto Field = Property->GetField();
	check(Field->Context->IsData());

	FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(Buffer.Buffer));
	UPsData* NewData = static_cast<UPsData*>(UPsDataUPsDataLibrary::TypeDeserialize(Property->GetOwner(), Field, &Deserializer, nullptr));

	if (Field->Context->IsArray())
	{
		const auto IndexOpt = Numbers::ToUnsignedInteger<int32>(ToStringView(Key));
		if (GetContext<TArray<UPsData*>>().IsA(Field->Context) && IndexOpt)
		{
			TPsDataArrayProxy<UPsData*> Proxy(static_cast<TDataProperty<TArray<UPsData*>>*>(Property));
			Proxy.Insert(NewData, IndexOpt.GetValue());
			return true;
		}
	}
	else if (Field->Context->IsMap())
	{
		if (GetContext<TMap<FString, UPsData*>>().IsA(Field->Context) && IsValidKey(Key))
		{
			TPsDataMapProxy<UPsData*> Proxy(static_cast<TDataProperty<TMap<FString, UPsData*>>*>(Property));
			if (!Proxy.Contains(Key))
			{
				Proxy.Add(Key, NewData);
				return true;
			}
		}
	}
	else
	{
		const auto DataProperty = static_cast<TDataProperty<UPsData*>*>(Property);
		DataProperty->SetValue(NewData);
		return true;
	}

	return false;
}

bool UPsNetworkData::ApplyRemovingEvent(FAbstractDataProperty* Property, const FString& Key) const
{
	const auto Field = Property->GetField();
	check(Field->Context->IsData());

	if (Field->Context->IsArray())
	{
		const auto IndexOpt = Numbers::ToUnsignedInteger<int32>(ToStringView(Key));
		if (GetContext<TArray<UPsData*>>().IsA(Field->Context) && IndexOpt)
		{
			TPsDataArrayProxy<UPsData*> Proxy(static_cast<TDataProperty<TArray<UPsData*>>*>(Property));
			if (Proxy.IsValidIndex(IndexOpt.GetValue()))
			{
				Proxy.RemoveAt(IndexOpt.GetValue());
				return true;
			}
		}
	}
	else if (Field->Context->IsMap())
	{
		if (GetContext<TMap<FString, UPsData*>>().IsA(Field->Context) && IsValidKey(Key))
		{
			TPsDataMapProxy<UPsData*> Proxy(static_cast<TDataProperty<TMap<FString, UPsData*>>*>(Property));
			if (Proxy.Contains(Key))
			{
				Proxy.Remove(Key);
				return true;
			}
		}
	}
	else
	{
		const auto DataProperty = static_cast<TDataProperty<UPsData*>*>(Property);
		DataProperty->SetValue(nullptr);
		return true;
	}

	return false;
}

void UPsNetworkData::MutableReset() const
{
	SynchronizePromise.Reset();
	const_cast<UPsNetworkData*>(this)->Reset();
}
