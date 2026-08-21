# ============================================================================
# BummerGram - Main Application Module
# ============================================================================

set(BG_CORE_SOURCES
    core/main.cpp
    core/main_window.cpp
    core/application.cpp
    core/localstorage.cpp
)

set(BG_CORE_HEADERS
    core/main_window.h
    core/application.h
    core/localstorage.h
)

set(BG_UI_SPLASH_SOURCES
    ui/splash/splash_screen.cpp
)

set(BG_UI_SPLASH_HEADERS
    ui/splash/splash_screen.h
)

set(BG_UI_SETTINGS_SOURCES
    ui/settings/bummer_settings_widget.cpp
    ui/settings/settings_general.cpp
    ui/settings/settings_lua_plugins.cpp
    ui/settings/settings_layout.cpp
)

set(BG_UI_SETTINGS_HEADERS
    ui/settings/bummer_settings_widget.h
    ui/settings/settings_general.h
    ui/settings/settings_lua_plugins.h
    ui/settings/settings_layout.h
)

set(BG_UI_WIDGETS_SOURCES
    ui/widgets/bummer_button.cpp
    ui/widgets/bummer_menu.cpp
    ui/widgets/message_context_actions.cpp
)

set(BG_UI_WIDGETS_HEADERS
    ui/widgets/bummer_button.h
    ui/widgets/bummer_menu.h
    ui/widgets/message_context_actions.h
)

set(BG_PLUGINS_SOURCES
    plugins/plugin_manager.cpp
    plugins/lua_state.cpp
    plugins/lua_safe_wrappers.cpp
    plugins/sandbox/lua_sandbox.cpp
)

set(BG_PLUGINS_HEADERS
    plugins/plugin_manager.h
    plugins/lua_state.h
    plugins/lua_safe_wrappers.h
    plugins/sandbox/lua_sandbox.h
)

set(BG_AUDIO_SOURCES
    audio/audio_player.cpp
    audio/startup_sound.cpp
)

set(BG_AUDIO_HEADERS
    audio/audio_player.h
    audio/startup_sound.h
)

# Combine all sources
set(BG_ALL_SOURCES
    ${BG_CORE_SOURCES}
    ${BG_UI_SPLASH_SOURCES}
    ${BG_UI_SETTINGS_SOURCES}
    ${BG_UI_WIDGETS_SOURCES}
    ${BG_PLUGINS_SOURCES}
    ${BG_AUDIO_SOURCES}
)

set(BG_ALL_HEADERS
    ${BG_CORE_HEADERS}
    ${BG_UI_SPLASH_HEADERS}
    ${BG_UI_SETTINGS_HEADERS}
    ${BG_UI_WIDGETS_HEADERS}
    ${BG_PLUGINS_HEADERS}
    ${BG_AUDIO_HEADERS}
)

# ============================================================================
# Build Executable
# ============================================================================

add_executable(BummerGram WIN32
    ${BG_ALL_SOURCES}
    ${BG_ALL_HEADERS}
    resources/qss/splash.qss
    resources/qss/settings.qss
)

# Include cmake modules
include(${CMAKE_SOURCE_DIR}/cmake/CompilerFlags.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)

# Link Qt6 libraries
target_link_libraries(BummerGram PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Multimedia
    ${LUA_LIBRARY}
    Threads::Threads
)

# Include directories
target_include_directories(BummerGram PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LUA_INCLUDE_DIR}
)

# Preprocessor definitions
target_compile_definitions(BummerGram PRIVATE
    BUMMERGRAM_VERSION_MAJOR=1
    BUMMERGRAM_VERSION_MINOR=0
    BUMMERGRAM_VERSION_PATCH=0
    BUMMERGRAM_VERSION_STRING="1.0.0"
    BUMMERGRAM_ORGANIZATION="BummerGram"
    BUMMERGRAM_APPLICATION="BummerGram"
    BUMMERGRAM_DATA_DIR="BummerGram"
)

# Windows resource file for version info
if(WIN32)
    configure_file(
        ${CMAKE_SOURCE_DIR}/bummergram/BummerGram.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/BummerGram.rc
        @ONLY
    )
    target_sources(BummerGram PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/BummerGram.rc)
endif()

# Debug: Print source files
message(STATUS "Core sources: ${BG_CORE_SOURCES}")
message(STATUS "UI Splash sources: ${BG_UI_SPLASH_SOURCES}")
message(STATUS "Plugins sources: ${BG_PLUGINS_SOURCES}")
