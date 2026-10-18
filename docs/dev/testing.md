# Building and testing the plugin

The repository is the plugin folder and nothing else; it carries no `.uproject`. To build it you
put a copy into a host project. `Tools/test.sh` does that for you on Linux and macOS.

## `Tools/test.sh`

```bash
Tools/test.sh /path/to/UnrealEngine                # build the editor target, run every PsData.* test
Tools/test.sh /path/to/UnrealEngine --game         # also build the monolithic Game target
Tools/test.sh /path/to/UnrealEngine --filter PsData.Events
UBT_ARGS=-NoUBA Tools/test.sh /path/to/UnrealEngine
```

It generates a minimal C++ host project (default `~/.cache/psdata-host/UE<major>.<minor>`, one per
engine version, so switching engines never rebuilds from scratch), copies the plugin into
`Plugins/PsData`, builds `PsDataHostEditor`, runs `Automation RunTests PsData` headless and prints
one line per test. The exit code is non-zero if anything failed to build or any test failed.

Measured on Linux with this tree: UE 5.8.1 and UE 5.6.1, 20 tests each, all passing; a warm editor
build is ~25 s. `--game` compiles the engine's modules for the host once (~10 minutes cold).

Why a copy and not a symlink: Unreal Build Accelerator tracks the symlinked path and the real path
as different files and aborts renaming an object file (`cross-process rename-while-open`). The
copy is refreshed with `rsync --delete` on every run, so the host always builds exactly the working
tree.

## By hand (Windows, or any host project)

1. Create a C++ project (or use an existing one) and copy this folder to
   `<Host>/Plugins/PsData`. `Binaries/`, `Intermediate/`, `docs/` and `Tools/` are not needed.
   Copy `Tests/PsDataTests` to `<Host>/Plugins/PsDataTests` and enable both plugins in the
   `.uproject`: left inside `Plugins/PsData/Tests` the test plugin is never found.
2. Build the editor target:

   ```bat
   <Engine>\Engine\Build\BatchFiles\Build.bat <Host>Editor Win64 Development -project=<Host>\<Host>.uproject
   ```

3. Run the suite and read the results from `<Host>/Saved/Logs/<Host>.log`:

   ```bat
   <Engine>\Engine\Binaries\Win64\UnrealEditor-Cmd.exe <Host>\<Host>.uproject -ExecCmds="Automation RunTests PsData; Quit" -unattended -nullrhi -nosplash -nopause
   ```

`; Quit` belongs **inside** the `Automation` command: `Automation` splits its own argument on `;`
and exits when the run completes, with the exit code taken from the results. Separated by a comma,
`Quit` becomes its own console command at frame 0 and the editor never exits.

Read the log, not stdout: the trace server forks and can clobber the tail of stdout.

## The suite

`Tests/PsDataTests` is a plugin of its own with one `DeveloperTool` module. It sits inside this
repository but outside the PsData plugin's discovery: the engine and UnrealBuildTool stop searching a
directory once they find a `.uplugin` in it (`FPluginManager::FindPluginsInDirectory`,
`EnumeratePlugins.cs`), so a project that installs PsData never compiles or loads the test classes.
`Tools/test.sh` copies it next to PsData in the host project and enables it. Its model (`PsDataTestModel.h`) is a miniature game — static
weapon data, characters with strict, event, alias, array and link fields under one root — and each
test file covers one area:

| Test | Covers |
| --- | --- |
| `PsData.Reflection`, `PsData.Properties` | reflection compile, meta parsing, defaults, operators, `Reset` |
| `PsData.Events.*` | field events, `EventStorage`, `IgnoreFieldMeta`, bubbling, `StopPropagation`, collection `Added`/`Removed`, the deferred `Changed`, `DEFERRED_EVENT_PROCESSING`, promises |
| `PsData.Tree`, `PsData.Collections` | keys and paths, parents and roots, map and array proxies, `Copy` |
| `PsData.Links` | resolution, re-resolution on key change, `Nullable`, dangling keys, `Validation` |
| `PsData.Serialization.RoundTrip`, `PsData.Hash` | JSON DOM, fast JSON, binary round trips, aliases on the wire, patch vs replace, content hash |
| `PsData.Serialization.Translate` | binary and JSON DOM documents translated to fast JSON without the model |
| `PsData.Types.Json.Formats` | `FPsDataJson` through every format and `Copy`: objects, arrays and scalars at the top level, int64 and 17-digit doubles, patch, change events |
| `PsData.Types.Json.Canonical` | compact storage, blank and invalid text become null, the document stays valid JSON, other spacing is not a change |
| `PsData.Serialization.Escapes` | `\uXXXX`, `\/` and short escapes from other writers; control characters, quotes and backslashes written so the engine's parser accepts them; escaped keys; 19-digit int64 |
| `PsData.Serialization.Truncated` | cut-off and empty fast-JSON documents: no out-of-bounds read, no assert, what arrived is applied |
| `PsData.DataTable` | `MAKE_TABLE_STRUCT` row struct generation and DataTable import |
| `PsData.Numbers.*` | `FPsDataFixedPoint`, `FPsDataBigInteger` |

Not covered: the Blueprint nodes (the editor module is built, not exercised), `UPsNetworkData`
replication, `DataSerializeAsync`, `FPsDataOptimizedBinarySerializer`.

**A test name must not be the prefix of another.** `PsData.Serialization` silently stopped running
the moment `PsData.Serialization.Translate` existed: the automation tree treats the shorter name as
the group. Give every test a leaf name (`PsData.Serialization.RoundTrip`) and check the count.

**A test is evidence once it has been seen to fail.** When a test pins down a fix, revert the fix,
watch that test fail, restore it. Two defects fixed in 2.1 were found exactly this way: a
`FPsDataFastJsonDeserializer` reading a destroyed temporary, and a `TPsDataPromise<T>` that did not
compile once instantiated.

## Engine versions

Version-dependent code goes through `PSDATA_ENGINE_AT_LEAST(Major, Minor)` from
`PsDataDefines.h`; never compare `ENGINE_MINOR_VERSION` alone (it resets every major version).
Container calls that take a shrink flag use `PSDATA_SHRINK(bAllow)` / `PSDATA_NO_SHRINK` (the
`bool` overloads were removed in 5.8). See `CHANGELOG.md` for what 5.8 changed.
