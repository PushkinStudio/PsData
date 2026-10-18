# Changelog

## 2.1 — Unreal Engine 5.8

Verified on UE 5.8.1 (editor, Game and Shipping targets) and UE 5.6.1 (editor), Linux, with the new
automation suite passing on both. The source keeps its compatibility branches for older engines;
those are not re-verified here.

### Ownership

- Pushkin Studio is the copyright holder of the project. Source headers and `LICENSE` now name
  Pushkin Studio instead of MY.GAMES; the license itself (MIT) is unchanged.

### Engine compatibility

- `UUserDefinedStruct` comes from `StructUtils/UserDefinedStruct.h` on 5.5 and later.
- `TArray` shrink arguments use `EAllowShrinking` on 5.4 and later (`PSDATA_SHRINK`,
  `PSDATA_NO_SHRINK`); 5.8 removed the `bool` overloads.
- JSON keys: `FJsonObject` stores keys as `UE::FSharedString` on 5.8; the JSON, struct and DataTable
  serializers no longer touch `FJsonObject::Values` by key.
- `FProperty` construction and `FField::Duplicate` drop object flags on 5.8, where fields no
  longer carry them.
- `FCoreDelegates::GetOnPostEngineInit()` on 5.8, `UK2Node::IsNodePure()` on 5.5 and later.
- `DMETA` no longer uses the deprecated `PREPROCESSOR_JOIN`, which warned once per `DMETA` in every
  project on 5.8.
- A `UE_LOG` with more arguments than format specifiers, which 5.8's format checker rejects at
  compile time, is fixed; an `int64` logged with `%d` now uses `%lld`.
- `PSDATA_ENGINE_AT_LEAST(Major, Minor)` is the version guard for all of the above.

### Fixes

- `FPsDataFastJsonDeserializer` kept a pointer into the string it was constructed from, so passing a
  temporary read freed memory. A new `FString&&` constructor takes such strings over; passing a
  named string still parses it in place.
- `TPsDataPromise<T>` did not compile once instantiated (wrong delegate array type, `TOptional::Get`
  without a default).
- `DMETA(CustomType)` removed the wrong tag from the parsed meta.
- `PsDataTools::IsValidKey(const T*)` rejected every non-empty string.

### Added

- `PsDataTests`, a `DeveloperTool` module with 15 automation tests (`PsData.*`) over a miniature
  game model: reflection, properties, events, tree, collections, links, serialization, hashing,
  DataTable import, fixed-point and big-integer numbers.
- `Tools/test.sh`: builds the plugin in a generated host project for any engine and runs the suite.
- Documentation: `docs/` (getting started, model reference, events, serialization, Blueprints,
  cookbook), `docs/ai-guide.md` for AI agents using the plugin, `AGENTS.md` for agents working on
  it, `llms.txt`.

### Known

- `Config/DefaultPsData.ini` still declares its pre-2019 module redirects with `MatchSubstring`,
  which UE 5.6+ logs as deprecated at startup. Switching to `MatchWildcard` changes how older
  engines read them, so it is left for a release that drops those engines.

## 2.0 and earlier

See the git history.
