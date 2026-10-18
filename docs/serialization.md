# Serialization, hashing, DataTables

Any node of the tree — the root or a single character — serializes with one call and loads back with
another. The format is chosen by the serializer object you pass:

```cpp
Data->DataSerialize(&Serializer);
Data->DataDeserialize(&Deserializer);          // reset, then load
Data->DataDeserialize(&Deserializer, true);    // patch: load over what is there
```

## Formats

| Serializer / deserializer | Wire form | Use it for |
| --- | --- | --- |
| `FPsDataFastJsonSerializer` / `FPsDataFastJsonDeserializer` | an `FString` of JSON, no DOM | save games, HTTP bodies, anything that is a string anyway |
| `FPsDataJsonSerializer` / `FPsDataJsonDeserializer` | a `TSharedPtr<FJsonObject>` | code that already speaks the engine's JSON DOM |
| `FPsDataBinarySerializer` / `FPsDataBinaryDeserializer` | bytes in an `FPsDataBufferOutputStream` | snapshots inside RPCs and transactions, no parsing |
| `FPsDataOptimizedBinarySerializer` / `...Deserializer` | bytes, keys replaced by indices into a shared `TArray<FString>` | repeated messages between peers that share the key dictionary |
| `FPsDataStructSerializer` / `FPsDataStructDeserializer` | a `USTRUCT` | moving data between PsData and engine structs, e.g. one DataTable row |
| `FPsDataTableDeserializer` | a `UDataTable` | static data authored in tables, see below |

```cpp
// Fast JSON
FPsDataFastJsonSerializer Serializer;           // FPsDataFastJsonSerializer(true) pretty-prints
Settings->DataSerialize(&Serializer);
const FString Json = Serializer.GetJsonString();

FPsDataFastJsonDeserializer Deserializer(Json);
Settings->DataDeserialize(&Deserializer);

// Binary
const auto Output = MakeShared<FPsDataBufferOutputStream>();
FPsDataBinarySerializer BinarySerializer(Output);
Room->DataSerialize(&BinarySerializer);
const TArray<uint8>& Bytes = Output->GetBuffer();

FPsDataBinaryDeserializer BinaryDeserializer(MakeShared<FPsDataBufferInputStream>(Bytes));
Room->DataDeserialize(&BinaryDeserializer);
```

What is written:

- Keys are field names, or the `Alias` when one is set. Renaming a C++ field behind an alias keeps
  old documents loading.
- Fields that still hold their default value are skipped (set `Serializer.bWriteDefaults = true` to
  keep them). Fields whose default differs because `InitProperties()` sets them are always written.
- `Hidden` fields are never written.
- Enums are written by name, `FName` lowercased, numbers as numbers, `FPsDataFixedPoint` and
  `FPsDataBigInteger` as exact strings.
- Data children are nested objects; `CustomType` children carry their class name.
- An unknown key while loading logs a warning in `LogData` and is skipped.

Binary is not automatically smaller than JSON: it writes keys and strings in full, so a
string-heavy model can come out larger (the test model: 456 bytes binary, 351 characters of JSON).
Its advantage is that nothing is parsed. Use the optimized form, or compress, when size matters.

**`FPsDataFastJsonDeserializer(const FString&)` parses the string in place and keeps pointing into
it**: the string must outlive the deserializer. Passing a temporary — a `TEXT("...")` literal or a
function's return value — is safe since 2.1, which takes such strings over (`FString&&` overload).

## Patching

`DataDeserialize(&Deserializer, true)` writes only the fields present in the document and leaves
the rest alone; without `true` the object is `Reset()` first, so absent fields go back to their
defaults and `InitProperties()` runs again.

```cpp
FPsDataFastJsonDeserializer Patch(TEXT("{\"Health\": 7}"));
Alice->DataDeserialize(&Patch, true);  // Health is 7, everything else untouched
```

This is how partial server responses are applied to client state in production: a meta-server
returns only what changed, and the client patches it into the live tree, firing exactly the events
for what changed.

## Content hash

`GetHash()` (an MD5 hex string) and `GetHashInt32()` describe content, not identity: equal data
hashes equal, any change anywhere below changes it, and restoring the content restores the hash.
Hashes are cached per node and invalidated along the path of a change, so asking again after a
small edit is cheap.

```cpp
// Save only when the content really changed, however many events fired
const int32 NewHash = Settings->GetHashInt32();
if (NewHash != LastSavedHash)
{
	LastSavedHash = NewHash;
	Save();
}
```

`Copy<T>()` deep-copies through the same machinery; the copy is detached and hashes equal.

## Asynchronous serialization

`DataSerializeAsync(&Serializer, Callback)` takes a binary snapshot on the game thread, converts
it to the serializer's format on a worker thread, and calls `Callback` back on the game thread.
The model can change in the meantime; the serializer cannot — keep it alive until the callback.

## DataTables

A class with `MAKE_TABLE_STRUCT` gets a generated row struct, `<ClassName>_DataTableRow` (the
`U` prefix dropped). It appears in the editor's row-struct picker, so designers author rows in a
normal DataTable, and code loads every row into a `DMAP` keyed by row name:

```cpp
UCLASS()
class UFooWeaponProto : public UPsData
{
	GENERATED_BODY()

public:
	DPROP(int32, Damage);

	DPROP(FPsDataFixedPoint, FireRate);

	MAKE_TABLE_STRUCT({
		Damage = 0;
		FireRate = 0;
	});
};
```

```cpp
// UFooWorldData: DMAP(UFooWeaponProto*, Weapons);
FPsDataTableDeserializer Deserializer(WeaponsTable, World->Weapons.Get());
World->DataDeserialize(&Deserializer, true);
```

The braces of `MAKE_TABLE_STRUCT` set the defaults the generated struct starts from. For a single
row, `FPsDataStructDeserializer(Table->GetRowStruct(), RowData)` loads one row into one object.

In the editor, a DataTable's context menu gains **Export as JSON (PsData)** and **Export as CSV
(PsData)**, which export through PsData's own struct serializer rather than the engine's.

## Validation

`Validation()` walks the subtree and returns one line per broken link — an empty path, a path that
leads nowhere, an empty key on a link without `Nullable`, a key that is not in the target map. It
never crashes, which makes it the thing to run after loading static data, in a cook commandlet or in
a test:

```cpp
for (const FString& Error : RootData->Validation())
{
	UE_LOG(LogFoo, Error, TEXT("%s"), *Error);
}
```

## Schema

`FPsDataSchemaJson::Get()` returns a JSON description of every PsData class and enum in the
process — field names, C++ types, content types. Feed it to a code generator for a backend or a
web client that has to speak the same model.

## Replication

`UPsNetworkData` is a data class that replicates its own subtree to chosen players, outside the
engine's property replication. Both sides call `OpenConnection(PlayerController)` — the server for
each player that should receive the subtree, the owning client with its local controller. The
server spawns a proxy actor owned by that controller, sends one binary snapshot once the client
confirms, then batched `Changed`/`Added`/`Removed` deltas at `NetUpdateFrequency`; the client's
`OnSynchronizePromise()` resolves when the snapshot has been applied. It is part of the plugin and compiles on every supported engine,
but the automation suite does not exercise it — test it in your own multiplayer setup before you
rely on it.
