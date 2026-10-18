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
- Fast JSON strings: `\uXXXX`, `\/`, `\b` and `\f` from other writers were kept as a backslash and
  letters, then doubled on the next write; control characters other than `\n\r\t` were written raw,
  which is invalid JSON. Both directions now follow RFC 8259, and keys are escaped like values.
- A truncated or empty fast-JSON document read past the end of its token list or hit `checkNoEntry`.
  It is now read up to where it stops; `FPsDataFastJsonDeserializer::IsComplete()` reports whether
  it was whole.

### Added

- `FPsDataJson`, a property type for any JSON value whose shape the model does not own (server
  notifications, free-form attributes). Kept as compact text, written as real JSON in JSON documents
  and as structure in binary; stored in compact form, and text that is not JSON is refused (null and
  an error) so a stored value can always be written. Whole numbers that fit an `int64` travel as
  `int64`, others as `double` at full precision. Brought over from the copy of PsData that DAWG ships, with its fast-JSON array
  handling fixed (an array inside a payload was read as an object there).
- `FPsDataDeserializer::Translate(FPsDataSerializer*)` copies a value with everything nested in it
  between any two formats without a model class, on top of a new `NextValueType()`.
- `double` in every serializer, deserializer and stream (`WriteValue(double)`, `ReadValue(double&)`,
  `WriteDouble`/`ReadDouble`). The new virtuals have default implementations, so serializers and
  streams written outside the plugin keep compiling.
- The JSON schema marks `FPsDataJson` fields with `IsDynamic`.
- `Tests/PsDataTests`, a separate test plugin with 20 automation tests (`PsData.*`) over a miniature
  game model: reflection, properties, events, tree, collections, links, serialization, translation,
  escapes, truncated input, `FPsDataJson`, hashing, DataTable import, fixed-point and big-integer
  numbers. Nested inside the PsData folder it is never discovered, so game projects never compile
  or load its classes; `Tools/test.sh` installs it next to PsData in the host project.
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
