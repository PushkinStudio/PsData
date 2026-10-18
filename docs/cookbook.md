# Cookbook

Patterns from games that ship on PsData — most visibly **DAWG**, a competitive tactical shooter
whose client, dedicated server and meta-server protocol all speak one PsData model. The code is
reduced to the pattern and renamed (`Foo`), but each recipe is the shape used in production.

## One root, mutation by role

The root holds every branch of the game's data. Branches that only one system may change are
declared with `DPROP_CONST`, which hands the mutable side to a single friend class and a const
view to everyone else — the compiler enforces who writes what.

```cpp
UCLASS()
class UFooRootData : public UPsDataRoot
{
	GENERATED_BODY()

	DPROP_CONST(UFooConfigData*, ConfigData, class FFooConfigDataAccessor);  // static data
	DPROP_CONST(UFooWorldData*, WorldData, class FFooWorldDataAccessor);     // static data
	DPROP_CONST(UFooUserData*, UserData, class FFooUserDataAccessor);        // server-authoritative profile
	DPROP_CONST(UFooBattleData*, BattleData, class FFooBattleDataAccessor);  // match state

	DMETA(Strict)
	DPROP(UFooSettingsData*, SettingsData);                                  // anyone may change settings

	DMETA(Strict)
	DPROP(UFooRuntimeData*, RuntimeData);                                    // client-only scratch state
};

class FFooWorldDataAccessor
{
private:
	friend class UFooDataSubsystem;  // the only writer of static data

	static UFooWorldData* GetMutable(UFooRootData* RootData)
	{
		return *RootData->WorldData.Mutable;
	}
};
```

Everyone reads `RootData->WorldData->Weapons`; only `UFooDataSubsystem` can write through
`FFooWorldDataAccessor::GetMutable(RootData)`.

## Static data from DataTables, validated at cook time

Designers own balance in DataTables; code loads every table into the `WorldData` branch once, when
PsData is ready, and links from live state into it.

```cpp
void UFooDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FDataDelegates::PostDataModuleInitPromise.Bind(FSimpleDelegate::CreateUObject(this, &UFooDataSubsystem::LoadStaticData));
}

void UFooDataSubsystem::LoadStaticData()
{
	RootData = NewObject<UFooRootData>(this);
	const auto World = FFooWorldDataAccessor::GetMutable(RootData);

	FPsDataTableDeserializer Weapons(WeaponsTable.LoadSynchronous(), World->Weapons.Get());
	World->DataDeserialize(&Weapons, true);

	FPsDataTableDeserializer Characters(CharactersTable.LoadSynchronous(), World->Characters.Get());
	World->DataDeserialize(&Characters, true);

	for (const FString& Error : RootData->Validation())
	{
		UE_LOG(LogFoo, Error, TEXT("Static data: %s"), *Error);
	}
}
```

DAWG also runs this load, with its validation, inside the cook commandlet and reports every problem
there, so a renamed row or a typo in a key is caught before a build ships rather than in a match.

## Settings that save themselves

Settings live in the tree like everything else. A single `Changed` listener saves them; the content
hash keeps a burst of edits — or an edit and its undo — from touching the disk.

```cpp
void UFooSettingsSubsystem::Load()
{
	if (const auto SaveGame = UFooSettingsSaveGame::Load())
	{
		FPsDataFastJsonDeserializer Deserializer(SaveGame->SettingsJson);
		Settings->DataDeserialize(&Deserializer);
	}
	LastSavedHash = Settings->GetHashInt32();

	Binds.Add(Settings->Bind(UPsDataEvent::Changed, FPsDataDelegate::CreateUObject(this, &UFooSettingsSubsystem::OnSettingsChanged)));
	Binds.Add(Settings->MasterVolume.Bind(FPsDataDelegate::CreateUObject(this, &UFooSettingsSubsystem::ApplyAudio)));
}

void UFooSettingsSubsystem::OnSettingsChanged(UPsDataEvent* Event)
{
	const int32 Hash = Settings->GetHashInt32();
	if (Hash == LastSavedHash)
	{
		return;
	}
	LastSavedHash = Hash;

	FPsDataFastJsonSerializer Serializer;
	Settings->DataSerialize(&Serializer);

	const auto SaveGame = UFooSettingsSaveGame::Create();
	SaveGame->SettingsJson = Serializer.GetJsonString();
	SaveGame->Save();
}
```

Adding a setting is one `DPROP` line: it is saved, loaded, defaulted by `InitProperties()` and
observable from that moment on.

## Server-to-client snapshots in one RPC

The match state is a subtree. The server serializes it to binary and sends it as one string inside
whatever transport the game already has (an RPC, a transaction, a reliable message); the client
deserializes into its own copy of the subtree, and every client-side listener fires for what
changed.

```cpp
// server
const auto Output = MakeShared<FPsDataBufferOutputStream>();
FPsDataBinarySerializer Serializer(Output);
Room->DataSerialize(&Serializer);
ClientSyncRoom(FBase64::Encode(Output->GetBuffer()));

// client
void AFooPlayerController::ClientSyncRoom_Implementation(const FString& Snapshot)
{
	TArray<uint8> Bytes;
	FBase64::Decode(Snapshot, Bytes);

	FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(Bytes));
	BattleData->Room->DataDeserialize(&Deserializer);
}
```

For continuous replication of a subtree instead of explicit snapshots, see `UPsNetworkData` in
[`serialization.md`](serialization.md#replication).

## A backend protocol made of data classes

Requests and responses to the meta-server are PsData classes too. One generic helper turns any of
them into JSON and back, so a new endpoint is a pair of class declarations.

```cpp
template <typename T>
TSharedPtr<FJsonObject> ToJson(const T* Value)
{
	FPsDataJsonSerializer Serializer;
	Serializer.bWriteDefaults = true;  // the backend sees every field, not just the non-default ones
	Value->DataSerialize(&Serializer);
	return Serializer.GetJson();
}

template <typename T>
T* FromJson(const TSharedPtr<FJsonObject>& Json)
{
	const auto Data = NewObject<T>();
	FPsDataJsonDeserializer Deserializer(Json);
	Data->DataDeserialize(&Deserializer);
	return Data;
}
```

Responses that update existing state go the other way: patch them into the live tree with
`DataDeserialize(&Deserializer, true)` and let the events drive the UI.
`FPsDataSchemaJson::Get()` describes every class for code generation on the backend side.

## Actors that mirror data

Gameplay actors do not own state; they point at data and follow it. Replicate only the key, look
the data up in the local tree on each side, bind, and let the data's lifetime drive the actor's.

```cpp
void AFooBomb::OnBombDataReady(const UFooBombData* InBombData)
{
	BombData = InBombData;
	Binds.Add(BombData->Bind(UPsDataEvent::Removed, FPsDataDelegate::CreateUObject(this, &AFooBomb::OnBombRemoved)));
	Binds.Add(BombData->State.Bind(FPsDataDelegate::CreateUObject(this, &AFooBomb::OnStateChanged)));
}

void AFooBomb::OnBombRemoved(UPsDataEvent* Event)
{
	Destroy();  // the data left the tree, so the actor goes too
}

void AFooBomb::EndPlay(const EEndPlayReason::Type Reason)
{
	Binds.Unbind();
	Super::EndPlay(Reason);
}
```

The same shape drives character meshes (bind the inventory's `Added`/`Removed` and the equipped
skin's changed event), HUD widgets and animation instances.

## Polymorphic children

Effects on a character are a map of different classes. `CustomType` writes the concrete class with
each element, so the map round-trips through every serializer as the right subclasses.

```cpp
DMETA(Event, CustomType)
DMAP(UFooEffectData*, Effects);  // holds UFooStunEffectData, UFooBurnEffectData, ...

template <class T>
TArray<const T*> GetEffects() const
{
	TArray<const T*> Result;
	for (const auto& Pair : Effects)
	{
		if (const auto Effect = Cast<T>(Pair.Value))
		{
			Result.Add(Effect);
		}
	}
	return Result;
}
```

## Local prediction next to authoritative state

A client predicts its own weapon state while the server's version keeps arriving. Both live in the
same character data; the predicted one is `Hidden`, so it never goes over the wire or into a save,
and `EventStorage` gives listeners the old value to animate from.

```cpp
DMETA(Event, EventStorage)
DPROP(FFooDeviceState, ServerDevice);

DMETA(Event, EventStorage, Hidden)
DPROP(FFooDeviceState, LocalDevice);
```

(`FFooDeviceState` is a custom value type — see [`model.md`](model.md#supported-types).)

## Waiting for readiness

Systems start in an order you do not fully control. A promise turns "is it ready yet?" into "run
this when it is", with no special case for "already happened":

```cpp
FPsDataSimplePromise SubmodulesReady;  // resolved by the game instance once every subsystem started

SubmodulesReady.Bind(FSimpleDelegate::CreateUObject(this, &UFooHud::Subscribe));
```

`FDataDelegates::PostDataModuleInitPromise` is the first link in that chain.
