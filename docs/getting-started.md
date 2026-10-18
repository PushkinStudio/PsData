# Getting started

From an empty project to a model you can change, observe and save. Every snippet on this page
compiles against the plugin as shipped; the same shapes run in its automation suite
(`Tests/PsDataTests`).

## 1. Install

PsData is a source plugin. Your project must be a C++ project.

```bash
cd <YourProject>/Plugins
git clone https://github.com/PushkinStudio/PsData.git
```

`PsData.uplugin` must end up at `<YourProject>/Plugins/PsData/PsData.uplugin`. Enable the plugin
(`Edit → Plugins`, or in the `.uproject`):

```json
"Plugins": [
    { "Name": "PsData", "Enabled": true }
]
```

and depend on the runtime module from every module that declares or uses data classes:

```csharp
// <YourModule>.Build.cs
PublicDependencyModuleNames.AddRange(
    new string[] { "Core", "CoreUObject", "Engine", "PsData" });
```

One include brings in the whole API:

```cpp
#include "PsDataAPI.h"
```

**Keep the plugin a real copy, not a symlink.** Unreal Build Accelerator aborts on a symlinked
plugin directory (`cross-process rename-while-open`). To keep the checkout outside the project, use
`"AdditionalPluginDirectories"` in the `.uproject` and point it at the *parent* of the `PsData`
folder.

## 2. Declare data

A data class is a `UCLASS` deriving from `UPsData`. Its members are declared with macros, not
`UPROPERTY`:

```cpp
// FooData.h
#pragma once

#include "PsDataAPI.h"

#include "FooData.generated.h"

UENUM(BlueprintType)
enum class EFooTeam : uint8
{
    None = 0,
    Red = 1,
    Blue = 2,
};
DESCRIBE_ENUM(EFooTeam);

UCLASS()
class UFooWeaponProto : public UPsData
{
    GENERATED_BODY()

public:
    DPROP(int32, Damage);

    DPROP(FPsDataFixedPoint, FireRate);
};

UCLASS()
class UFooCharacterData : public UPsData
{
    GENERATED_BODY()

public:
    DPROP(FString, Name);

    DMETA(Event)
    DPROP(int32, Health);

    DMETA(Event)
    DPROP(EFooTeam, Team);

    DARRAY(FString, Tags);

    DPROP(FString, Weapon);
    DLINK(UFooWeaponProto*, Weapon, World.Weapons);

protected:
    virtual void InitProperties() override
    {
        Health = 100;
    }
};
```

- `DPROP(Type, Name)` declares a typed property; `DARRAY` and `DMAP` declare an array and a
  string-keyed map.
- `DMETA(...)` on the line before a declaration configures it. `Event` means *changes to this field
  are broadcast*.
- `DLINK(ReturnType, KeyField, Path)` turns a key field into a typed reference to data elsewhere in
  the tree — here, `Weapon` holds a key into `World.Weapons`.
- `InitProperties()` is the constructor for data: it runs on creation and on every `Reset()`.

The full macro, type and meta reference is [`model.md`](model.md).

## 3. Give it a root

Links resolve from a root, and the root is what ties static data and live state into one tree:

```cpp
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
    DMAP(UFooCharacterData*, Characters);
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
};
```

`Strict` children are allocated with their owner and can never be null or replaced.

## 4. Build the model — after PsData is ready

PsData compiles its reflection once, at `PostEngineInit`. **In a packaged game that is after
`UGameInstance::Init` and every game-instance subsystem's `Initialize`** (`UGameEngine::Init`
starts the game instance inside `GEngine->Init`, before the engine loop broadcasts
`PostEngineInit`). Code that runs that early must wait:

```cpp
void UFooDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Runs at once if PsData is already initialised, otherwise when it is
    FDataDelegates::PostDataModuleInitPromise.Bind(
        FSimpleDelegate::CreateUObject(this, &UFooDataSubsystem::BuildModel));
}

void UFooDataSubsystem::BuildModel()
{
    RootData = NewObject<UFooRootData>(this);

    const auto Rifle = NewObject<UFooWeaponProto>();
    Rifle->Damage = 30;
    Rifle->FireRate = FPsDataFixedPoint(TEXT("10.5"));
    RootData->World->Weapons->Add(TEXT("rifle"), Rifle);

    const auto Alice = NewObject<UFooCharacterData>();
    Alice->Name = TEXT("Alice");
    Alice->Weapon = TEXT("rifle");
    RootData->Battle->Characters->Add(TEXT("alice"), Alice);
}
```

`RootData` is a `UPROPERTY()` of the subsystem: data objects are ordinary `UObject`s and the root
must be referenced to stay alive. Everything below the root is kept alive by the tree.

## 5. Read, write, observe

```cpp
UFooCharacterData* Alice = RootData->Battle->Characters[TEXT("alice")];

Alice->Health -= 30;                      // properties behave like their value type
const int32 Health = *Alice->Health;      // * reads the value
const bool bHurt = Alice->Health < 100;   // comparisons and arithmetic work in place

const UFooWeaponProto* Weapon = Alice->LinkByWeapon.Get(); // the link resolves "rifle"
const int32 Damage = *Weapon->Damage;

// Health has Event meta: this runs synchronously on every change
Binds.Add(Alice->Health.Bind(
    FPsDataDelegate::CreateUObject(this, &UFooHealthBar::OnHealthChanged)));
```

`Binds` is an `FPsDataBindCollection` member: it unbinds everything it holds when it is destroyed
or when you call `Binds.Unbind()` — call that in `EndPlay`/`Deinitialize`. Events, bubbling and the
deferred `Changed` event are in [`events.md`](events.md).

## 6. Save and load

```cpp
// Save: one compact JSON string
FPsDataFastJsonSerializer Serializer;
RootData->Battle->DataSerialize(&Serializer);
const FString Json = Serializer.GetJsonString();

// Load into existing data
FPsDataFastJsonDeserializer Deserializer(Json);
RootData->Battle->DataDeserialize(&Deserializer);
```

JSON DOM, binary, patching, content hashes and DataTable import are in
[`serialization.md`](serialization.md).

## Where next

- [`model.md`](model.md) — every macro, meta tag and supported type.
- [`events.md`](events.md) — field events, bubbling, collection events, deferral.
- [`serialization.md`](serialization.md) — formats, patching, hashing, DataTables, validation.
- [`blueprints.md`](blueprints.md) — what designers get in the graph editor.
- [`cookbook.md`](cookbook.md) — patterns from shipped games.
