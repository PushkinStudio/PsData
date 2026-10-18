# PsData for AI coding agents

This page is for an agent writing **game code that uses PsData** (Claude Code, Codex, Cursor,
Copilot, …). An agent working on the plugin itself reads [`AGENTS.md`](../AGENTS.md) instead.

PsData suits agents: the model is declared in plain C++ headers, every field is one greppable line,
and behaviour is configured by short meta tags rather than editor state. The catch is that several
mistakes compile cleanly and then either do nothing or stop the process with `Fatal` at run time.
The rules below are the ones that cost real debugging time.

To brief an agent in your project, add this to its `CLAUDE.md` / `AGENTS.md`:

```markdown
Game data uses the PsData plugin (reactive data model, Plugins/PsData). Before declaring or
changing data classes, read Plugins/PsData/docs/ai-guide.md and Plugins/PsData/docs/model.md.
```

## Rules

1. **Declare model members with the macros, never `UPROPERTY`.** A data class is
   `UCLASS() class UFooData : public UPsData { GENERATED_BODY() ... }` with `DPROP`, `DARRAY`,
   `DMAP`, `DLINK`; include `"PsDataAPI.h"` and the class's `.generated.h`. A `UPROPERTY` on a data
   class is invisible to serialization, events, links and the Blueprint nodes.

2. **Use the accessor syntax.** Read with `*Data->Field`, write with `Data->Field = Value`.
   Collections go through `->`: `Data->Items->Add(X)`, `Data->Map->Remove(Key)`,
   `Data->Map[TEXT("key")]`, `for (const auto& Pair : Data->Map)`. There is no mutable reference to
   the underlying container; every write goes through the proxy so it can broadcast.

3. **No `Event` meta, no event.** `Data->Field.Bind(...)` on a field declared without
   `DMETA(Event)` compiles and never fires. Add `DMETA(Event)` on the line directly above the
   `DPROP`, or bind with `EDataBindFlags::IgnoreFieldMeta`. Add `Bubbles` only when an ancestor
   needs to hear it. `Strict` and `Event` cannot be combined.

4. **`UPsDataEvent::Changed` is deferred and per object.** It fires once, after the current code
   returns, on the object whose own field changed — not on its parents. For a synchronous reaction
   to one field, bind that field.

5. **Map keys are `[A-Za-z0-9_-]` only.** A key with a dot, space, slash or colon is `Fatal` in
   development builds the moment the map is set. Never use display names, emails or composite ids
   as keys without mapping them to safe strings first.

6. **Links need a root and a resolvable key.** `LinkByX.Get()` walks from `GetRoot()`: the owner
   must be attached under a `UPsDataRoot`, and a link without `DMETA(Nullable)` whose key is empty
   or absent is `Fatal` on access. Mark optional links `Nullable` and null-check the result. After
   loading data, run `Root->Validation()` — it reports every broken link without crashing.

7. **Build the model after PsData is ready.** Reflection compiles at `PostEngineInit`, and in a
   packaged game `UGameInstance::Init` and game-instance subsystems run *before* that. Create model
   objects inside `FDataDelegates::PostDataModuleInitPromise.Bind(...)`, which runs at once if
   PsData is already initialised. Code that works in PIE and fails in a packaged build is the
   symptom of getting this wrong.

8. **One parent per data object.** Adding an object that already has a parent is `Fatal`. Move it
   by removing it first, or add `Original->Copy<UFooData>()`.

9. **Unbind what you bind.** Keep every `FPsDataBind` in an `FPsDataBindCollection` member and call
   `Unbind()` in `EndPlay` / `Deinitialize` / `NativeDestruct`. A lambda capturing `this` that
   outlives its actor is a crash waiting for the next change.

10. **Keep the root alive.** Data objects are `UObject`s. Hold the root in a `UPROPERTY()` of a
    subsystem or game instance; children are kept alive by the tree.

11. **Defaults are not serialized.** A field still at its default value is left out of JSON and
    binary. When another system needs every field (a backend contract, a diff tool), set
    `Serializer.bWriteDefaults = true`.

12. **Renaming a field is a wire change.** Saved games, server payloads and DataTables use field
    names as keys. Keep the old key with `DMETA(Alias = OldName)` when renaming, and expect the
    Blueprint nodes for that field to need replacing.

13. **Game thread only.** No PsData call from a worker thread; `DataSerializeAsync` is the one
    supported way to move serialization work off the game thread.

14. **Set defaults in `InitProperties()`**, not in a constructor. It runs on creation and on every
    `Reset()`, including the reset that precedes a non-patch `DataDeserialize`.

## Verifying a change

- **It compiles** is not evidence: rules 3, 5, 6 and 7 all compile.
- **Write an automation test** next to your model, shaped like the plugin's own
  (`Plugins/PsData/Source/PsDataTests`): build a small tree, bind, mutate, assert counts, round-trip
  through a serializer, compare `GetHash()`. `PsData.Events.Deferred` shows how to flush the
  deferred `Changed` event inside a test
  (`FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread)`).
- **Run `Root->Validation()`** after loading data and fail loudly on any line it returns.
- **Inspect a class at run time** with the console command `PsData.TypeInfo UFooData`: its fields,
  links and every path in the tree that leads to it.
- **Grep before you add.** `grep -rn "DPROP(.*, Health)" Source/` finds every declaration; the
  event name is always `<Field>Changed` unless the meta says `Event = Name`.

## Map of the docs

| Need | Page |
| --- | --- |
| Macro, meta tag or type reference | [`model.md`](model.md) |
| Which event fires when | [`events.md`](events.md) |
| Saving, loading, network payloads, DataTables, hashes | [`serialization.md`](serialization.md) |
| What Blueprints can do with the model | [`blueprints.md`](blueprints.md) |
| A shape that already works in production | [`cookbook.md`](cookbook.md) |
