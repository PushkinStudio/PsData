<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/assets/psdata-wordmark-dark.svg" />
    <img src="docs/assets/psdata-wordmark-light.svg" alt="PsData" width="380" />
  </picture>
</p>

<p align="center">
  <strong>A reactive data model for Unreal Engine.</strong><br />
  Declare game state, configs and protocol messages as typed C++ classes —<br />
  get events that bubble, links between branches, JSON and binary for free.
</p>

<p align="center">
  <a href="docs/getting-started.md">Get started</a> &nbsp;|&nbsp;
  <a href="#a-tour">Tour</a> &nbsp;|&nbsp;
  <a href="docs/cookbook.md">Cookbook</a> &nbsp;|&nbsp;
  <a href="#documentation">Docs</a> &nbsp;|&nbsp;
  <a href="#ai-agents">AI agents</a>
</p>

<p align="center">
  <a href="#compatibility"><img src="https://img.shields.io/badge/Unreal%20Engine-5.6%20%C2%B7%205.8-1e2630?style=flat-square&logo=unrealengine&logoColor=white" alt="Unreal Engine 5.6 and 5.8" /></a>
  <a href="docs/dev/testing.md"><img src="https://img.shields.io/badge/tests-20%20passing-2f7d5b?style=flat-square" alt="20 automation tests passing" /></a>
  <a href="docs/blueprints.md"><img src="https://img.shields.io/badge/C%2B%2B%20%C2%B7%20Blueprints-1e2630?style=flat-square" alt="C++ and Blueprints" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-1e2630?style=flat-square" alt="MIT license" /></a>
</p>

Games keep a lot of data that is not actors: the player's profile, the match state, balance tables,
settings, the messages exchanged with a backend. PsData gives that data one home — a tree of typed
objects you declare in a few lines of C++ — and makes the tree **observable**: every change can be
heard where it happened or anywhere above it, links resolve keys into the right branch, and any
node serializes to JSON or binary and loads back with a patch.

It was built at Pushkin Studio and runs in production: in **DAWG**, a competitive tactical shooter,
the client, the dedicated server and the meta-server protocol all speak one PsData model.

## The model

One match, declared once. Everything else on this page — events, links, saving, the network,
Blueprint nodes — comes from these macros.

```cpp
UCLASS()
class UFooCharacterData : public UPsData
{
    GENERATED_BODY()

public:
    DPROP(FString, Name);

    /** Broadcast on change, with the old value */
    DMETA(Event, EventStorage)
    DPROP(int32, Health);

    /** Renamed in C++, same key on the wire */
    DMETA(Alias = "hp_max")
    DPROP(int32, MaxHealth);

    /** No Event meta: silent unless a listener asks */
    DPROP(int32, Kills);

    DMETA(Event)
    DPROP(EFooTeam, Team);

    /** Allocated with its owner, never null */
    DMETA(Strict)
    DPROP(UFooPointData*, Location);

    DARRAY(FString, Tags);

    /** A key, resolved into static data by the link below */
    DPROP(FString, Weapon);
    DLINK(UFooWeaponProto*, Weapon, World.Weapons);

    /** May point at nothing */
    DPROP(FString, Target);
    DMETA(Nullable)
    DLINK(UFooCharacterData*, Target, Battle.Characters);

    /** Any subclass of UFooEffectData, saved with its class name */
    DMETA(Event, CustomType)
    DMAP(UFooEffectData*, Effects);

protected:
    /** Defaults; runs again on every Reset() */
    virtual void InitProperties() override
    {
        Health = 100;
        MaxHealth = 100;
    }
};
```

<details>
<summary><b>The rest of the model</b> — weapons for a DataTable, positions that bubble, effects, settings, the root</summary>

```cpp
UENUM()
enum class EFooTeam : uint8
{
    None,
    Red,
    Blue,
};
DESCRIBE_ENUM(EFooTeam);

UCLASS()
class UFooWeaponProto : public UPsData
{
    GENERATED_BODY()

public:
    DPROP(int32, Damage);

    /** "10.5" is exactly 10.5 */
    DPROP(FPsDataFixedPoint, FireRate);

    /** Generates FooWeaponProto_DataTableRow, the row struct designers pick */
    MAKE_TABLE_STRUCT({
        Damage = 0;
        FireRate = 0;
    });
};

UCLASS()
class UFooPointData : public UPsData
{
    GENERATED_BODY()

public:
    /** Heard by every ancestor */
    DMETA(Event, Bubbles)
    DPROP(int32, X);

    DMETA(Event, Bubbles)
    DPROP(int32, Y);
};

UCLASS()
class UFooEffectData : public UPsData
{
    GENERATED_BODY()

public:
    DPROP(FPsDataFixedPoint, Duration);
};

UCLASS()
class UFooStunEffectData : public UFooEffectData
{
    GENERATED_BODY()
};

UCLASS()
class UFooWorldData : public UPsData
{
    GENERATED_BODY()

public:
    DMAP(UFooWeaponProto*, Weapons);
};

UCLASS()
class UFooBattleData : public UPsData
{
    GENERATED_BODY()

public:
    DMETA(Event)
    DPROP(int32, Round);

    DMAP(UFooCharacterData*, Characters);
};

UCLASS()
class UFooSettingsData : public UPsData
{
    GENERATED_BODY()

public:
    DMETA(Event)
    DPROP(float, MasterVolume);

    DMETA(Event)
    DPROP(FString, Language);
};

UCLASS()
class UFooConfigData : public UPsData
{
    GENERATED_BODY()

public:
    DPROP(int32, RoundSeconds);
};

UCLASS()
class UFooRootData : public UPsDataRoot
{
    GENERATED_BODY()

public:
    DMETA(Strict)
    DPROP(UFooWorldData*, World);

    DMETA(Strict)
    DPROP(UFooBattleData*, Battle);

    DMETA(Strict)
    DPROP(UFooSettingsData*, Settings);

    /** Only UFooDataSubsystem may write it */
    DPROP_CONST(UFooConfigData*, Config, class UFooDataSubsystem);
};
```

</details>

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/assets/model-dark.svg" />
    <img src="docs/assets/model-light.svg" alt="A PsData tree: a root with a static World branch holding weapon prototypes and a live Battle branch holding characters. A change to a character's position bubbles up to the battle; the character's Weapon key links across into World.Weapons." width="100%" />
  </picture>
</p>

No `UPROPERTY`, no getters and setters, no `OnRep`, no hand-written JSON. The supported types are
numbers, strings, names, text, enums, vectors, colours, curves, soft references, fixed-point and big
integers, free-form JSON, arrays and string-keyed maps of any of them, and child data
([`docs/model.md`](docs/model.md)).

## A tour

One match, start to finish. Every snippet runs against the model above.

### React to what changed

Listeners are plain delegates. Bind to one field, to an event that bubbles up from anywhere below,
or to a collection:

```cpp
// HUD: a health bar. The event carries the old and the new value
Binds.Add(Alice->Health.Bind(
    FPsDataDelegate::CreateLambda([this, Alice](UPsDataEvent* Event) {
        const auto Change = Alice->Health.GetEventStorage(Event);
        HealthBar->Animate(Change->GetPreviousValue(), Change->GetValue());
    })));

// HUD: a minimap. One listener on the battle hears every character move,
// because X and Y bubble up
Binds.Add(Battle->Bind(UFooPointData::GetXChangedEventName(),
    FPsDataDelegate::CreateLambda([this](UPsDataEvent* Event) {
        const auto Character = Cast<UFooCharacterData>(Event->GetTarget()->GetParent());
        Minimap->Move(Character->GetDataKey(),
            *Character->Location->X, *Character->Location->Y);
    })));

// Game mode: pawns follow the data, spawned and destroyed with their characters
Binds.Add(Battle->Characters->Bind(UPsDataEvent::Added,
    FPsDataDelegate::CreateUObject(this, &AFooGameMode::SpawnPawn)));
Binds.Add(Battle->Characters->Bind(UPsDataEvent::Removed,
    FPsDataDelegate::CreateUObject(this, &AFooGameMode::DestroyPawn)));
```

`Binds` is an `FPsDataBindCollection`; everything in it unbinds when it is destroyed. Every object
also sends one deferred `Changed` per burst of edits — the event to hang saving and UI refreshes on.
→ [`docs/events.md`](docs/events.md)

### Change many things as one

```cpp
void AFooGameMode::StartRound()
{
    // Listeners run after the block, and see the finished state
    DEFERRED_EVENT_PROCESSING();

    ++Battle->Round;
    for (const auto& Pair : Battle->Characters)
    {
        Pair.Value->Health = *Pair.Value->MaxHealth;
        Pair.Value->Effects->Empty();
    }
}
```

### Keys in, objects out

Live state stores keys; links resolve them through the tree, cache the result and follow when
either side changes.

```cpp
// World.Weapons[Alice->Weapon]
const UFooWeaponProto* Weapon = Alice->LinkByWeapon.Get();
const int32 Damage = *Weapon->Damage;

// Nullable: may be empty
if (const UFooCharacterData* Target = Alice->LinkByTarget.Get())
{
    UE_LOG(LogTemp, Log, TEXT("%s aims at %s"), **Alice->Name, **Target->Name);
}

// The link follows the key; nothing to re-resolve by hand
Alice->Weapon = TEXT("shotgun");

// Every dangling key in the tree, without a crash
for (const FString& Error : RootData->Validation())
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
}
```

### Decide who may write

```cpp
// Anyone reads...
const int32 Seconds = *RootData->Config->RoundSeconds;

// ...but this does not compile: Config is const outside its friend
// RootData->Config->RoundSeconds = 90;

void UFooDataSubsystem::ApplyServerConfig(int32 RoundSeconds)
{
    // The friend named in DPROP_CONST gets the mutable side
    UFooConfigData* Config = *RootData->Config.Mutable;
    Config->RoundSeconds = RoundSeconds;
}
```

### Save only what really changed

```cpp
// Settings save themselves: Changed comes once per burst of edits,
// and the content hash skips edits that cancel out
Binds.Add(Settings->Bind(UPsDataEvent::Changed,
    FPsDataDelegate::CreateLambda([this](UPsDataEvent*) {
        if (Settings->GetHashInt32() == SavedHash)
        {
            return;
        }
        SavedHash = Settings->GetHashInt32();

        FPsDataFastJsonSerializer Json;
        Settings->DataSerialize(&Json);
        FFileHelper::SaveStringToFile(Json.GetJsonString(), *SettingsPath);
    })));

// And back, at startup
FString Saved;
if (FFileHelper::LoadFileToString(Saved, *SettingsPath))
{
    FPsDataFastJsonDeserializer Json(Saved);
    Settings->DataDeserialize(&Json);
}
```

### Ship state over the wire

```cpp
// Server: the battle as bytes, through any RPC or transaction you already have
const auto Bytes = MakeShared<FPsDataBufferOutputStream>();
FPsDataBinarySerializer Out(Bytes);
Battle->DataSerialize(&Out);
PlayerController->Client_SyncBattle(Bytes->GetBuffer());

// Client: the same subtree, rebuilt; everything bound to it updates through its events
void AFooPlayerController::Client_SyncBattle_Implementation(const TArray<uint8>& Snapshot)
{
    FPsDataBinaryDeserializer In(MakeShared<FPsDataBufferInputStream>(Snapshot));
    Battle->DataDeserialize(&In);
}

// A partial update from a backend, e.g. {"Health": 40, "Team": "Blue"}:
// only the fields it names are touched, and only real changes fire
FPsDataFastJsonDeserializer Patch(ResponseBody);
Alice->DataDeserialize(&Patch, true);
```

### Carry JSON you do not own

`FPsDataJson` holds any JSON value. A server push can carry per-character changes of any shape,
and each one patches the character it names:

```cpp
UCLASS()
class UFooNotifyData : public UPsData
{
    GENERATED_BODY()

public:
    /** Character key -> whatever changed */
    DMAP(FPsDataJson, Updates);
};

for (const auto& Pair : Notify->Updates)
{
    if (UFooCharacterData* const* Character = Battle->Characters->Find(Pair.Key))
    {
        FPsDataFastJsonDeserializer Update(Pair.Value.GetJsonString());
        (*Character)->DataDeserialize(&Update, true);
    }
}
```

### Read any snapshot

```cpp
// A binary snapshot from a crash report or a replay, as JSON a human can read;
// no model class needed
FPsDataBinaryDeserializer In(MakeShared<FPsDataBufferInputStream>(Snapshot));
FPsDataFastJsonSerializer Pretty(true);
In.Translate(&Pretty);
UE_LOG(LogTemp, Display, TEXT("%s"), *Pretty.GetJsonString());
```

### Let designers own the numbers

```cpp
// UFooWeaponProto has MAKE_TABLE_STRUCT, so designers pick FooWeaponProto_DataTableRow
// as the row struct of an ordinary DataTable
FPsDataTableDeserializer Rows(WeaponsTable, RootData->World->Weapons.Get());
RootData->World->DataDeserialize(&Rows, true);

// Fixed point: "10.5" is exactly 10.5, and the result is the same on every machine
const FPsDataFixedPoint ShotsPerMinute = *Weapon->FireRate * FPsDataFixedPoint(60);
```

### Any subclass, by name

```cpp
Alice->Effects->Add(TEXT("stun"), NewObject<UFooStunEffectData>());

// CustomType writes the class with the value,
// so a copy, a save or a snapshot comes back as a stun
const auto Copy = Alice->Copy<UFooCharacterData>();
const bool bStillStun = Copy->Effects[TEXT("stun")]->IsA<UFooStunEffectData>();
```

### And from Blueprints

None of this needs glue code to reach a graph. Every field gets **Get** and **Set** nodes (no Set
for `Strict`, `ReadOnly` or `DPROP_CONST` fields), every link a node, every collection a proxy, and
**Bind** offers a dropdown of every event that can reach the object, bubbling ones included.
→ [`docs/blueprints.md`](docs/blueprints.md)

## In production

PsData runs DAWG's client, dedicated server and meta-server protocol. The
[cookbook](docs/cookbook.md) has those shapes end to end: one root with role-gated branches, static
data from DataTables validated at cook time, settings that save themselves, binary snapshots inside
transactions, a backend protocol made of data classes, and actors that mirror data.

## Install

PsData is a source plugin; your project must be a C++ project.

```bash
cd <YourProject>/Plugins
git clone https://github.com/PushkinStudio/PsData.git
```

Enable it in `Edit → Plugins` (or `"Plugins": [{ "Name": "PsData", "Enabled": true }]` in the
`.uproject`), add `"PsData"` to your module's `PublicDependencyModuleNames`, and
`#include "PsDataAPI.h"`. The first editor launch compiles the plugin.

Use a copy or a clone, not a symlink: Unreal Build Accelerator aborts on symlinked plugin folders.
To keep the checkout outside your project, point `AdditionalPluginDirectories` in the `.uproject`
at the folder *containing* `PsData`.

**Build the model after PsData is ready.** Its reflection compiles at `PostEngineInit`, which in a
packaged game comes *after* `UGameInstance::Init`; create model objects from
`FDataDelegates::PostDataModuleInitPromise.Bind(...)`. [Details](docs/getting-started.md#4-build-the-model--after-psdata-is-ready).

## Documentation

| Page | What it answers |
| --- | --- |
| [Getting started](docs/getting-started.md) | **Start here.** Install, declare a model, give it a root, read, write, observe, save. |
| [Model](docs/model.md) | Every macro, meta tag and supported type; the tree API; the rules that are enforced with fatal errors. |
| [Events](docs/events.md) | Which event fires when, bubbling, the deferred `Changed`, holding events back. |
| [Serialization](docs/serialization.md) | Formats, patching, content hashes, DataTables, validation, schema, replication. |
| [Blueprints](docs/blueprints.md) | The nodes the editor generates and how to bind from a graph. |
| [Cookbook](docs/cookbook.md) | Production patterns, end to end. |
| [Changelog](CHANGELOG.md) | What changed, including the UE 5.8 port. |

## AI agents

The model is plain C++ headers — one greppable line per field — which suits coding agents. The
mistakes that matter compile cleanly and then do nothing or stop the game at run time, so the
docs spell them out for agents:

- **[`docs/ai-guide.md`](docs/ai-guide.md)** — for an agent writing game code *with* PsData: the
  fourteen rules (events need `Event` meta, map-key alphabet, links need a root, when the model may
  be built, …) and how to verify a change. Paste this into your project's `CLAUDE.md` / `AGENTS.md`:

  ```markdown
  Game data uses the PsData plugin (reactive data model, Plugins/PsData).
  Before declaring or changing data classes, read
  Plugins/PsData/docs/ai-guide.md and Plugins/PsData/docs/model.md.
  ```

- **[`AGENTS.md`](AGENTS.md)** — for an agent working *on* the plugin: one command to build and
  test, the layout, the rules for changes. `CLAUDE.md` imports it.
- **[`llms.txt`](llms.txt)** — an index of the docs and the key sources for LLM tools.

## Compatibility

| Engine | Status |
| --- | --- |
| UE 5.8 | Verified on 5.8.1: editor, Game and Shipping targets build with no plugin warnings; 20/20 tests pass. |
| UE 5.6 | Verified on 5.6.1: editor target builds with no plugin warnings; 20/20 tests pass. |
| UE 5.7 | Not verified here; the version guards cover it. |
| Earlier | The source keeps upstream's compatibility branches back to 4.24; they are not re-verified. |

Verified on Linux. The plugin has no platform-specific code.

## Testing

```bash
Tools/test.sh /path/to/UnrealEngine
```

Builds the plugin in a generated host project for that engine and runs the `PsData.*` automation
suite — 20 tests over a miniature game model. Windows steps and coverage:
[`docs/dev/testing.md`](docs/dev/testing.md).

## License

MIT — see [`LICENSE`](LICENSE). Created by Nikolay Prudnikov at Pushkin Studio; with Vladimir
Alyamkin, Maxim Fedorenko, Anton Rassadin and Aleksey Ilyin.
