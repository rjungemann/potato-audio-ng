cmake_minimum_required (VERSION 3.25.0)

set (CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "")

project (PotatoAudioNG
	VERSION 1.0.0.0
	DESCRIPTION "Potato Audio NG - VST3 plug-in collection"
)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (APPLE)
	enable_language (OBJCXX)
endif ()

# Fetches the VST3 SDK (with VSTGUI) and RtAudio via CPM.cmake.
add_subdirectory (external)

smtg_enable_vst3_sdk ()

# ---------------------------------------------------------------------------
# VST3 plug-in target
# ---------------------------------------------------------------------------
smtg_add_vst3plugin (PotatoNoise
	source/version.h
	source/PluginIDs.h
	source/NoiseEngine.h
	source/PluginProcessor.h
	source/PluginProcessor.cpp
	source/PluginController.h
	source/PluginController.cpp
	source/PluginEntry.cpp
	resource/editor.uidesc
)

target_link_libraries (PotatoNoise
	PRIVATE
		sdk
		vstgui_support
)

smtg_target_add_plugin_resources (PotatoNoise
	RESOURCES
		resource/editor.uidesc
)

smtg_target_configure_version_file (PotatoNoise)

if (SMTG_MAC)
	smtg_target_set_bundle (PotatoNoise
		BUNDLE_IDENTIFIER com.potatoaudio.potatonoise
		COMPANY_NAME "Potato Audio"
	)
	smtg_target_set_debug_executable (PotatoNoise
		"/Applications/VST3PluginTestHost.app"
		"--pluginfolder;$(BUILT_PRODUCTS_DIR)"
	)
elseif (SMTG_WIN)
	target_sources (PotatoNoise PRIVATE resource/win32resource.rc)
	if (MSVC)
		set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PotatoNoise)
	endif ()
endif ()

# ---------------------------------------------------------------------------
# Standalone application target
# ---------------------------------------------------------------------------
set (standalone_target PotatoNoiseStandalone)

vstgui_add_executable (${standalone_target}
	"source/standalone/StandaloneApp.cpp;source/NoiseEngine.h"
)
vstgui_add_resources (${standalone_target} "resource/editor.uidesc")
vstgui_set_target_bundle_id (${standalone_target} "com.potatoaudio.potatonoise")
vstgui_set_target_infoplist (${standalone_target} "resource/StandaloneInfo.plist")
vstgui_set_target_rcfile (${standalone_target} "resource/standalone.rc")
vstgui_set_cxx_version (${standalone_target} 17)

# VSTGUI's own executables always add their vstgui4 root manually (see
# vstgui/standalone/examples/*/CMakeLists.txt) - vstgui_add_executable does not
# propagate it, so headers like "vstgui/standalone/include/iapplication.h" would
# otherwise not resolve.
target_include_directories (${standalone_target}
	PRIVATE
		${vst3sdk_SOURCE_DIR}/vstgui4
)

target_link_libraries (${standalone_target}
	PRIVATE
		rtaudio
)
