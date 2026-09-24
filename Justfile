# Potato Audio NG - developer tasks.
# Run `just` (no arguments) to list all available recipes.

build_dir := "build"
config := "Debug"

plugin_target := "PotatoNoise"
standalone_target := "PotatoNoiseStandalone"

plugin_bundle := build_dir / "VST3" / config / (plugin_target + ".vst3")
standalone_app := build_dir / "bin" / config / (standalone_target + ".app")
validator_bin := build_dir / "bin" / config / "validator"

default:
    @just --list

# Configure the CMake build (Ninja generator).
#
# On macOS with only the Xcode Command Line Tools installed (no full Xcode.app),
# `xcodebuild -version` fails and the VST3 SDK's CMake refuses to configure. We
# detect that case and feed it a plausible version through its documented
# XCODE_VERSION override; when a real Xcode.app is present its own detection is
# left alone.
configure:
    #!/usr/bin/env bash
    set -euo pipefail
    xcode_override=()
    if [[ "$(uname)" == "Darwin" ]] && ! xcodebuild -version >/dev/null 2>&1; then
        export XCODE_VERSION="16.0"
        xcode_override=(-DXCODE_VERSION=16.0)
    fi
    cmake -B {{build_dir}} -G Ninja -DCMAKE_BUILD_TYPE={{config}} "${xcode_override[@]}"

# Build everything (plug-in + standalone app). Also runs the VST3 validator and
# (re)installs the plug-in symlink into the user VST3 folder, both handled by
# the SDK's own CMake integration.
build: configure
    cmake --build {{build_dir}} --config {{config}}

# Build only the VST3 plug-in target.
build-plugin: configure
    cmake --build {{build_dir}} --config {{config}} --target {{plugin_target}}

# Build only the standalone application target.
build-standalone: configure
    cmake --build {{build_dir}} --config {{config}} --target {{standalone_target}}

# Run the VST3 validator against the built plug-in.
validate: build-plugin
    "{{validator_bin}}" "{{plugin_bundle}}"

# Launch the standalone application.
run: build-standalone
    open "{{standalone_app}}"

# The VST3 SDK automatically symlinks the built .vst3 into the user's local
# VST3 folder as part of `build-plugin` - this just makes that explicit/visible.
install: build-plugin
    @echo "Installed (symlinked) to: $(ls -l ~/Library/Audio/Plug-Ins/VST3/{{plugin_target}}.vst3 2>/dev/null | awk '{print $NF}')"

# Remove all build artifacts.
clean:
    rm -rf {{build_dir}}

# Wipe and reconfigure from scratch.
reconfigure: clean configure
