#!/usr/bin/env bash
# Builds PsData inside a throwaway host project and runs its automation suite.
#
#   Tools/test.sh <engine-root> [options]
#
#   <engine-root>    the directory that holds Engine/ (a source build or a launcher install)
#   --host <dir>     where the host project lives (default: ~/.cache/psdata-host/UE<major>.<minor>)
#   --filter <name>  automation filter (default: PsData)
#   --game           also build the monolithic Game target (Development), which compiles the engine
#                    for the host once and takes a while; it is the build a packaged game uses
#   --no-tests       build only
#
# Extra arguments for UnrealBuildTool go in UBT_ARGS, e.g. UBT_ARGS=-NoUBA.
# Linux and macOS. On Windows run the same three steps by hand (docs/dev/testing.md).

set -euo pipefail

usage() { sed -n '2,15p' "$0" | sed 's/^# \{0,1\}//'; exit "${1:-0}"; }

[[ $# -ge 1 ]] || usage 1
case "$1" in -h|--help) usage 0 ;; esac

ENGINE=$(cd "$1" && pwd)
shift
HOST=""
FILTER="PsData"
BUILD_GAME=0
RUN_TESTS=1
while [[ $# -gt 0 ]]; do
	case "$1" in
		--host) HOST="$2"; shift 2 ;;
		--filter) FILTER="$2"; shift 2 ;;
		--game) BUILD_GAME=1; shift ;;
		--no-tests) RUN_TESTS=0; shift ;;
		-h|--help) usage 0 ;;
		*) echo "Unknown option: $1" >&2; usage 1 ;;
	esac
done

PLUGIN=$(cd "$(dirname "$0")/.." && pwd)
VERSION_FILE="$ENGINE/Engine/Build/Build.version"
[[ -f "$VERSION_FILE" ]] || { echo "Not an engine root (no Engine/Build/Build.version): $ENGINE" >&2; exit 1; }
MAJOR=$(sed -n 's/.*"MajorVersion": *\([0-9]*\).*/\1/p' "$VERSION_FILE")
MINOR=$(sed -n 's/.*"MinorVersion": *\([0-9]*\).*/\1/p' "$VERSION_FILE")
PATCH=$(sed -n 's/.*"PatchVersion": *\([0-9]*\).*/\1/p' "$VERSION_FILE")

case "$(uname -s)" in
	Linux) PLATFORM=Linux; EDITOR_CMD="$ENGINE/Engine/Binaries/Linux/UnrealEditor-Cmd" ;;
	Darwin) PLATFORM=Mac; EDITOR_CMD="$ENGINE/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor" ;;
	*) echo "Unsupported OS; see docs/dev/testing.md for the manual steps" >&2; exit 1 ;;
esac

HOST=${HOST:-"${XDG_CACHE_HOME:-$HOME/.cache}/psdata-host/UE$MAJOR.$MINOR"}
NAME=PsDataHost
echo "PsData $PLUGIN"
echo "Engine $ENGINE (UE $MAJOR.$MINOR.$PATCH, $PLATFORM)"
echo "Host   $HOST"

# The host is generated, never edited: a C++ game module that depends on PsData and nothing else.
mkdir -p "$HOST/Source/$NAME" "$HOST/Config" "$HOST/Plugins"
cat > "$HOST/$NAME.uproject" <<EOF
{
	"FileVersion": 3,
	"EngineAssociation": "",
	"Modules": [{ "Name": "$NAME", "Type": "Runtime", "LoadingPhase": "Default" }],
	"Plugins": [{ "Name": "PsData", "Enabled": true }, { "Name": "PsDataTests", "Enabled": true }]
}
EOF
for TARGET in Game Editor; do
	SUFFIX=$([[ $TARGET == Editor ]] && echo Editor || true)
	cat > "$HOST/Source/$NAME$SUFFIX.Target.cs" <<EOF
using UnrealBuildTool;

public class $NAME${SUFFIX}Target : TargetRules
{
	public $NAME${SUFFIX}Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.$TARGET;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("$NAME");
	}
}
EOF
done
cat > "$HOST/Source/$NAME/$NAME.Build.cs" <<EOF
using UnrealBuildTool;

public class $NAME : ModuleRules
{
	public $NAME(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "PsData" });
	}
}
EOF
cat > "$HOST/Source/$NAME/$NAME.cpp" <<EOF
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, $NAME, "$NAME");
EOF
printf '[/Script/EngineSettings.GameMapsSettings]\nGameDefaultMap=/Engine/Maps/Entry\n' > "$HOST/Config/DefaultEngine.ini"

# A copy, not a symlink: Unreal Build Accelerator aborts on a symlinked plugin
# ("cross-process rename-while-open"). Only what the build reads is copied.
rsync -a --delete \
	--exclude .git --exclude Binaries --exclude Intermediate --exclude docs --exclude Tools --exclude Tests \
	"$PLUGIN/" "$HOST/Plugins/PsData/"

# The suite is its own plugin: nested inside PsData it is never discovered (plugin discovery stops at
# the first .uplugin), so game projects never get its classes. Here it sits next to PsData.
rsync -a --delete --exclude Binaries --exclude Intermediate \
	"$PLUGIN/Tests/PsDataTests/" "$HOST/Plugins/PsDataTests/"

build() {
	local Target=$1
	echo "Building $Target..."
	local Log="$HOST/Saved/build-$Target.log"
	mkdir -p "$HOST/Saved"
	if ! "$ENGINE/Engine/Build/BatchFiles/$PLATFORM/Build.sh" "$Target" "$PLATFORM" Development \
		-project="$HOST/$NAME.uproject" ${UBT_ARGS:-} > "$Log" 2>&1; then
		grep -E "error|Error" "$Log" | head -40 || true
		echo "Build of $Target failed; full log: $Log" >&2
		exit 1
	fi
	# Plugin warnings are bugs here: show them, they are not noise
	grep -E "Plugins/PsData/.*warning" "$Log" | sed 's#^.*/Plugins/PsData/##' | sort -u || true
	tail -n 2 "$Log" | head -n 1
}

build "${NAME}Editor"
[[ $BUILD_GAME -eq 1 ]] && build "$NAME"
[[ $RUN_TESTS -eq 1 ]] || exit 0

echo "Running automation: $FILTER"
LOG="$HOST/Saved/Logs/$NAME.log"
rm -f "$LOG"
# "Automation RunTests X; Quit" — Automation splits its own argument on ';' and quits when the run ends
# with an exit code from the results. A comma-separated Quit would run at frame 0 and never fire.
set +e
"$EDITOR_CMD" "$HOST/$NAME.uproject" -ExecCmds="Automation RunTests $FILTER; Quit" \
	-unattended -nullrhi -nosplash -nopause -nosound > /dev/null 2>&1
EXIT=$?
set -e

# Read results from the log, not stdout: a forked trace server can clobber the tail of stdout
grep -E "Test Completed\. Result=" "$LOG" | sed -E 's/.*Result=\{([A-Za-z]+)\} Name=\{[^}]*\} Path=\{([^}]*)\}.*/  \1  \2/' || true
grep -E "LogAutomationController: Error:" "$LOG" | grep -v "Test Completed" | sed 's/^.*Error: /    /' || true
PASSED=$(grep -c "Result={Success}" "$LOG" || true)
FAILED=$(grep -c "Result={Fail}" "$LOG" || true)
echo "UE $MAJOR.$MINOR.$PATCH: $PASSED passed, $FAILED failed (editor exit code $EXIT)"
[[ $EXIT -eq 0 && $FAILED -eq 0 && $PASSED -gt 0 ]]
