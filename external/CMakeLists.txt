include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# ---------------------------------------------------------------------------
# VST3 SDK (includes VSTGUI as the "vstgui4" submodule)
# ---------------------------------------------------------------------------
# Keep the plugin/hosting example projects and doc/tutorials submodules out of
# the build - we only need the SDK libraries, pluginterfaces and VSTGUI.
set (SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
set (SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
set (SMTG_ENABLE_VSTGUI_SUPPORT ON CACHE BOOL "" FORCE)
set (VSTGUI_STANDALONE_EXAMPLES OFF CACHE BOOL "" FORCE)
set (VSTGUI_TOOLS OFF CACHE BOOL "" FORCE)

CPMAddPackage(
	NAME vst3sdk
	GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
	GIT_TAG v3.8.1_build_84
	GIT_SUBMODULES "base;cmake;pluginterfaces;public.sdk;vstgui4"
)

# ---------------------------------------------------------------------------
# RtAudio - lightweight cross-platform audio I/O, used only by the standalone
# application target to open a real output device.
# ---------------------------------------------------------------------------
set (BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set (RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)

CPMAddPackage(
	NAME rtaudio
	GIT_REPOSITORY https://github.com/thestk/rtaudio.git
	GIT_TAG 6.0.1
)

# CPM/FetchContent only set these in the scope they were called from. The
# standalone target (defined in the top-level CMakeLists.txt) needs the VSTGUI
# headers, which live under vst3sdk's "vstgui4" submodule.
set (vst3sdk_SOURCE_DIR "${vst3sdk_SOURCE_DIR}" PARENT_SCOPE)
