# Declaring the model

Everything you declare with PsData is a node in one tree. A data class is a `UCLASS` deriving
from `UPsData` (or `UPsDataRoot` for the top of the tree); its members are declared with the macros
below. The macros generate the storage, the typed accessors, the change events, the reflection used
by serializers and Blueprints, and the parent/child bookkeeping.

```cpp
UCLASS()
class UFooCharacterData : public UPsData
{
    GENERATED_BODY()

public:
    DMETA(Event, EventStorage)
    DPROP(int32, Health);
    ...
};
```

## Macros

| Macro | Declares | Notes |
| --- | --- | --- |
| `DPROP(Type, Name)` | a property | `Type` is a value type or a `UPsData` subclass pointer (a child node). |
| `DARRAY(Type, Name)` | `TArray<Type>` | Elements that are data become children keyed by index. |
| `DMAP(Type, Name)` | `TMap<FString, Type>` | Keys are strings; elements that are data become children keyed by the map key. |
| `DLINK(ReturnType, KeyField, Path)` | `LinkBy<KeyField>`, a typed reference | `KeyField` is a `DPROP`/`DARRAY` of keys declared just above; `Path` is a dotted path from the root to a `DMAP` (`World.Weapons`). An array of keys links to an array of values. |
| `DLINK_ABSTRACT(ReturnType, KeyField)` | `LinkByAbstract<KeyField>` | A link declared without a path, typically in a base class; the path comes from a regular `DLINK` on the same key field (`FDataReflection::InitLinkProperty` in `PsDataCore.cpp`). Advanced. |
| `DPROP_CONST(Type, Name, Friend)` | a property only `Friend` can mutate | Implies `ReadOnly, Strict`. `Friend` reaches the mutable value through `Name.Mutable`; everyone else gets a const view. |
| `DMETA(...)` | meta for the next declaration | Comma-separated tags, see below. |
| `MAKE_TABLE_STRUCT({ defaults })` | a generated DataTable row struct | See [`serialization.md`](serialization.md#datatables). |
| `DESCRIBE_ENUM(EType)` | PsData support for a `UENUM` | Put it right after the enum, at namespace scope. |
| `DEFERRED_EVENT_PROCESSING()` | an event scope guard | See [`events.md`](events.md#holding-events-back). |

Every `DPROP` also generates a static `Get<Name>ChangedEventName()`, which is the event type to
bind to by name (`UFooCharacterData::GetHealthChangedEventName()` is `"HealthChanged"`).

## Using properties

A property member behaves like the value it holds:

```cpp
// Assign
Character->Health = 100;

// Compound assignment; ++ and -- work on arithmetic types too
Character->Health -= 30;

// Read with operator*
const int32 Value = *Character->Health;

// Compare arithmetic types in place
if (Character->Health > 50) {}

// -> reaches members of a struct or string value
const bool bNoName = Character->Name->IsEmpty();

// -> on a data pointer walks the tree
Character->Location->X = 5;
```

Collections expose their API through `->`:

```cpp
Character->Tags->Add(TEXT("sniper"));
Character->Tags->RemoveAt(0);
const int32 Count = Character->Tags->Num();

// Alice becomes a child, with the key "alice"
Battle->Characters->Add(TEXT("alice"), Alice);

// Bob is detached from the tree
Battle->Characters->Remove(TEXT("bob"));

UFooCharacterData* Found = Battle->Characters[TEXT("alice")];

// A pointer to the element, nullptr if absent
const auto Maybe = Battle->Characters->Find(TEXT("nobody"));

// Pair.Key, Pair.Value
for (const auto& Pair : Battle->Characters)
{
}
```

A collection write replaces the stored container and broadcasts once. Writes go through the proxy;
there is no mutable reference to the underlying `TArray`/`TMap`.

## Meta

| Tag | Applies to | Effect |
| --- | --- | --- |
| `Event` | property | Changes broadcast `<Name>Changed`. Without it a change is silent unless someone binds with `EDataBindFlags::IgnoreFieldMeta` ([`events.md`](events.md)). |
| `Event = Custom` | property | Same, with a custom event type name. |
| `Bubbles` | property with `Event` | The event travels up through every ancestor. |
| `EventStorage` | property with `Event` | The event carries the previous and the new value. |
| `Strict` | data property | Allocated with its owner; never null, never replaced, not settable from Blueprints. Combined with `Event`, the `Event` tag is dropped with an error log: a strict child never changes identity. |
| `Alias = name` | property | The key used on the wire (JSON/binary/DataTable). Old documents keep loading after a C++ rename. |
| `ReadOnly` | property | No Blueprint setter. On a data property, it makes every field of that class read-only too. |
| `Hidden` | property | Not serialized, not replicated, not in the generated row struct. Local-only state. |
| `Deprecated` | property | Blueprint nodes for it are marked deprecated. |
| `CustomType` | data property | Serializes the concrete class name, so the field can hold any subclass of its declared type. |
| `Nullable` | `DLINK` | The key may be empty or point at nothing; the link is then `nullptr` instead of a fatal error. |

Meta is read once per class and shared by all instances. A tag the parser does not know is ignored
(logged at `VeryVerbose` in `LogDataReflection`).

## Supported types

| Kind | Types |
| --- | --- |
| Numbers | `int32`, `int64`, `uint8`, `float`, `bool` |
| Deterministic numbers | `FPsDataFixedPoint` (fixed point, exact decimal parsing), `FPsDataBigInteger` (arbitrary precision) |
| Text | `FString`, `FName`, `FText` |
| Enums | any `UENUM` with `DESCRIBE_ENUM` — serialized by name |
| Engine structs | `FVector`, `FIntVector`, `FLinearColor`, `FRuntimeFloatCurve` |
| Assets | `TSoftObjectPtr<T>`, `TSoftClassPtr<T>` |
| Free-form JSON | `FPsDataJson` — any JSON value, kept as text and written as real JSON ([`serialization.md`](serialization.md#free-form-json-fpsdatajson)) |
| Data | `UFooData*` for any `UPsData` subclass |
| Containers | `TArray<T>` via `DARRAY`, `TMap<FString, T>` via `DMAP`, for any `T` above |

**Your own value type** is a copy of an existing one: take `Types/PsData_FVector.h` and
`Types/PsData_FVector.cpp`, rename the type, and keep the four specializations
(`TDataTypeContext`, `TTypeSerializer`, `TTypeDeserializer`, `TTypeComparator`) and the
`UPsData<Type>Library` with its custom thunks — that library is what makes the type visible to the
Blueprint nodes.

## Lifecycle hooks

```cpp
protected:
    /**
     * Runs when the object is created and on every Reset().
     * Set defaults here, not in a constructor
     */
    virtual void InitProperties() override;

    /** Runs after creation and after every deserialization; derive cached values here */
    virtual void PostDeserialize() override;
```

`Reset()` returns every property to its default and runs `InitProperties()` again.
`Copy<T>()` returns a detached deep copy (no parent, no root).

## The tree

```cpp
Alice->GetParent();          // the data that owns it (UFooBattleData)
Alice->GetRoot();            // the UPsDataRoot at the top, if attached to one
Alice->GetDataKey();         // "alice": the map key, or the index for array elements
Alice->GetCollectionKey();   // "Characters": the field it lives in
Alice->GetFullDataKey();     // "Characters.alice"
Alice->GetPathFromRoot();    // "Battle.Characters.alice"
```

A data object has exactly one parent. Adding an object that already has a parent is a fatal error;
remove it from its old place first, or add a `Copy()`.

## Rules that are enforced

These fail loudly — most with `UE_LOG(..., Fatal, ...)` in non-shipping builds:

- **Map keys** may contain only `A–Z a–z 0–9 _ -`. Anything else (a space, a dot, a slash) is fatal
  when the map is set.
- **Links** need a root: `LinkByX.Get()` walks from `GetRoot()`, so the owner must be attached
  under a `UPsDataRoot`. A non-`Nullable` link whose key is empty or missing is fatal on access;
  `Validation()` reports the same problems without crashing.
- **Strict cycles**: a class that `Strict`-allocates itself, directly or through other classes,
  would allocate forever and is fatal at reflection compile time.
- **One parent**: see above.

## Reflection lifecycle

PsData builds its reflection from the class default objects of every `UPsData` subclass, once, at
`PostEngineInit`; `FDataDelegates::PostDataModuleInitPromise` resolves when it is done. In a
packaged game `UGameInstance::Init` runs *before* that point, so build the model from
`PostDataModuleInitPromise.Bind(...)` (it runs immediately if the promise is already resolved).

`PsData.TypeInfo <Class>` in the console prints a class's fields, links and every path that leads
to it.
