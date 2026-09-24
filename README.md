# Potato Audio NG

A VST3 plug-in built with C++, CMake, [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake), the
[VST3 SDK](https://github.com/steinbergmedia/vst3sdk) and [VSTGUI](https://github.com/steinbergmedia/vstgui).
Ships both as a VST3 plug-in and as a standalone application.

First plug-in: **Potato Noise**, a white noise generator with a single **Amplitude** knob
(0..1, defaults to 0).

## Requirements

- CMake >= 3.25
- Ninja
- [`just`](https://github.com/casey/just)
- A C++17 compiler (Xcode Command Line Tools are enough on macOS - see note below)
- Git (VST3 SDK and RtAudio are fetched on first configure via CPM.cmake)

## Quickstart

```sh
just build          # configure + build the plug-in and the standalone app
just run             # launch the standalone app
just validate         # run Steinberg's VST3 validator against the plug-in
just install           # (re)install the plug-in - see note below, this already happens automatically
```

Run `just` with no arguments to list all recipes.

## Project layout

```
CMakeLists.txt          top-level build: fetches deps, defines both targets
Justfile                 developer tasks (build, run, validate, clean, ...)
cmake/CPM.cmake            vendored CPM.cmake script
external/CMakeLists.txt      fetches vst3sdk (incl. VSTGUI) and RtAudio via CPM
source/
  NoiseEngine.h             plain C++ DSP core (no VST3/VSTGUI dependency) - shared by both targets
  PluginIDs.h                parameter IDs and class UIDs
  PluginProcessor.{h,cpp}     Steinberg::Vst::AudioEffect - audio processing
  PluginController.{h,cpp}     Steinberg::Vst::EditControllerEx1 - parameters + editor
  PluginEntry.cpp             VST3 factory (plug-in only)
  standalone/StandaloneApp.cpp  standalone app: VSTGUI::Standalone window + RtAudio output
resource/
  editor.uidesc              shared VSTGUI editor layout (used by BOTH the plug-in and the standalone app)
  StandaloneInfo.plist         standalone .app bundle Info.plist (macOS)
  win32resource.rc / standalone.rc  Windows resource files (untested, included for completeness)
```

The DSP (`NoiseEngine`) has zero VST3/VSTGUI dependencies, and the VSTGUI editor layout
(`resource/editor.uidesc`) is loaded by both the plug-in (via `VSTGUI::VST3Editor`) and the
standalone app (via `VSTGUI::Standalone::UIDesc::makeWindow`), so there is exactly one DSP
implementation and one UI layout to maintain.

The standalone app does **not** go through the VST3 hosting API - it drives `NoiseEngine`
directly from an [RtAudio](https://github.com/thestk/rtaudio) callback, and reuses the plug-in's
`editor.uidesc` through VSTGUI's own `Standalone` application framework.

## Notes / gotchas discovered while building this

- **Xcode Command Line Tools only (no full Xcode.app):** the VST3 SDK's CMake calls
  `xcodebuild -version` to gate on "Xcode 9+" and fails hard if only the Command Line Tools are
  installed. `just configure` detects this and passes the SDK's documented `XCODE_VERSION`
  override (both as an env var *and* a `-D` cache variable - both are required, see the
  `configure` recipe). If you have full Xcode.app installed, this is skipped and the SDK detects
  your real version as normal.
- **Plug-in auto-install:** the VST3 SDK's `smtg_target_set_bundle()` already symlinks the built
  `.vst3` into `~/Library/Audio/Plug-Ins/VST3/` as part of every plug-in build - `just install` is
  just there for discoverability, it doesn't do anything extra.
- **VSTGUI standalone executables need their include root added manually.** `vstgui_add_executable`
  links the `vstgui` / `vstgui_uidescription` / `vstgui_standalone` targets but does not propagate
  the VSTGUI source root as a usage requirement (Steinberg's own standalone examples add it by hand
  too, e.g. `target_include_directories(target PRIVATE ../../../../)`). The top-level
  `CMakeLists.txt` does this via `${vst3sdk_SOURCE_DIR}/vstgui4`.
- The VST3 validator (`build/bin/Debug/validator`) runs automatically as part of every plug-in
  build.

## Adding a second plug-in

This repo is set up for one plug-in (`PotatoNoise`). To add another, duplicate the
`smtg_add_vst3plugin(...)` block in `CMakeLists.txt` (new target name, new source files, new
`PluginIDs.h`-style UID header) and give it its own `resource/*.uidesc`.
