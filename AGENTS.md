# Working on PsData

Orientation for a coding agent (and for a human who has just cloned this) **changing the plugin
itself**. If you are writing game code that *uses* PsData, read
[`docs/ai-guide.md`](docs/ai-guide.md) instead.

PsData is a reactive data model for Unreal Engine: game data declared as C++ classes with macros
(`DPROP`, `DARRAY`, `DMAP`, `DLINK`), with change events that bubble up the tree, typed links between
branches, JSON/binary serialization, DataTable import, content hashing and generated Blueprint
nodes. Source plugin, MIT, UE 5.6–5.8 verified.

## Verify your change

```bash
Tools/test.sh <engine-root>            # build the editor target in a throwaway host, run PsData.* tests
Tools/test.sh <engine-root> --game     # also the monolithic Game target — do this before a release
```

The script prints one line per test and exits non-zero on any build error or failed test. It also
prints every compiler warning located in the plugin: **a plugin warning is a bug**, not noise —
deprecations become errors one engine later. Details, Windows steps and the coverage table:
[`docs/dev/testing.md`](docs/dev/testing.md).

If more than one engine is installed, run the script on the newest and the oldest one you support;
version guards that are only compiled on one side rot silently.

## Layout

```
PsData.uplugin
Source/
  PsData/                    Runtime module — everything a game links
    Public/PsDataAPI.h         the macros (DPROP, DMETA, DLINK, MAKE_TABLE_STRUCT, ...) and TDProp/TDLink
    Public/PsData.h            UPsData: tree, events, serialize entry points
    Public/PsDataCore.h        FDataReflection: field registry, compile at PostEngineInit
    Public/PsDataProperty.h    TDataProperty<T>: storage, SetValue, event storage
    Public/PsDataLink.h        link resolution and caching
    Public/PsDataDefines.h     PSDATA_ENGINE_AT_LEAST, PSDATA_SHRINK, FProperty helpers
    Public/Collection/         array and map proxies (C++ and Blueprint)
    Public/Serialize/          JSON, fast JSON, binary, optimized binary, struct, DataTable, schema
    Public/Types/              one file pair per supported value type
    Public/PsNetworkData.h     UPsNetworkData replication
  PsDataEditor/              UncookedOnly — Blueprint nodes, pin factory, details customizations
Tests/PsDataTests/           a separate plugin: the automation suite and its test model. Nested inside
                             PsData it is never discovered, so game projects never get its classes;
                             Tools/test.sh installs it next to PsData in the host project
Config/                      CoreRedirects for pre-2019 module names
Resources/Icon128.png        plugin browser icon
Tools/test.sh                build + test runner
Tools/gen_model_diagram.py   regenerates docs/assets/model-*.svg (README diagram)
docs/                        user documentation; docs/dev/ is for contributors; docs/assets/ images,
                             docs/assets/brand/ logo notes, concept sheet, social preview
```

## Rules that are not obvious from the code

1. **Engine-version code goes through `PSDATA_ENGINE_AT_LEAST(Major, Minor)`** (`PsDataDefines.h`).
   Never test `ENGINE_MINOR_VERSION` alone. Keep the older branch compiling: the plugin is used on
   more than one engine at a time.
2. **No `bool` shrink arguments.** `RemoveAt`, `Pop`, `SetNum` take `PSDATA_NO_SHRINK` or
   `PSDATA_SHRINK(bAllow)`; the `bool` overloads are gone in 5.8.
3. **No direct `FJsonObject::Values` access by key.** Keys are `FString` before 5.8 and
   `UE::FSharedString` since. Use `SetField`/`TryGetField`, and `FString(*Pair.Key)` when iterating.
4. **Preserve line endings.** Some files are CRLF (`PsDataDefines.h`, `PsDataStruct.h`,
   `PsDataLink.h`, `PsDataUtils.h`, `PsNetworkData.*`, `PsDataFixedPoint.*`, ...), most are LF.
   Edit in place; do not normalize a file you are not otherwise rewriting.
5. **Format with the repo's `Source/.clang-format`** (tabs, Allman braces, no column limit).
   `clang-format --style=file:Source/.clang-format -i <file>` on files you touch.
6. **The wire format is a contract.** Field keys, enum-by-name, binary token values and the
   fast-JSON layout are read back by saved games and live servers. A change there needs a
   migration story and a `CHANGELOG.md` entry.
7. **Game thread only.** The event scope guard and delegate tables are unsynchronized statics.
   Don't introduce PsData calls on other threads; `DataSerializeAsync` converts an already-taken
   snapshot off-thread and is the pattern to copy.
8. **A fix comes with a test, and the test has been seen to fail.** Add it to `Tests/PsDataTests`
   (one file per area), revert the fix, watch it fail, restore. Name tests `PsData.<Area>.<Case>`,
   and never give a test the name of another test's group (`PsData.Serialization` next to
   `PsData.Serialization.Translate`): the shorter one silently stops running. Check the count.
9. **Docs are part of the change.** Public API or behaviour changes update the matching page in
   `docs/` and, if user-visible, `CHANGELOG.md`. Code samples in docs must compile against the
   plugin — the safest source for a sample is a test. In Markdown code blocks indent with four
   spaces (GitHub draws a tab eight wide), put comments above the line they explain — `/** */`
   over a property, `//` over a statement — and keep lines within 90 characters, which is what
   GitHub shows without a scrollbar.
10. **New source files** start with the repository's copyright line
    (`// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.`) like their neighbours.

## Docs map

| Page | For |
| --- | --- |
| [`README.md`](README.md) | front page |
| [`docs/getting-started.md`](docs/getting-started.md) | install to first saved model |
| [`docs/model.md`](docs/model.md) | macros, meta, types, tree, enforced rules |
| [`docs/events.md`](docs/events.md) | event semantics |
| [`docs/serialization.md`](docs/serialization.md) | formats, patching, hashing, DataTables, replication |
| [`docs/blueprints.md`](docs/blueprints.md) | generated nodes |
| [`docs/cookbook.md`](docs/cookbook.md) | production patterns |
| [`docs/ai-guide.md`](docs/ai-guide.md) | agents *using* the plugin |
| [`docs/dev/testing.md`](docs/dev/testing.md) | building and testing the plugin |
| [`llms.txt`](llms.txt) | index for LLM tools |
| [`CHANGELOG.md`](CHANGELOG.md) | what changed per version |
