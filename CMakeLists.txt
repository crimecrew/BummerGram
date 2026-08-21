cmake_minimum_required(VERSION 3.20)
project(BummerGram VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# Build Configuration
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)

# Platform-specific settings
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    add_compile_definitions(WINVER=0x0A00)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

# MSVC Configuration
if(MSVC)
    add_compile_options(/W4 /WX- /permissive- /MP)
    add_link_options(/DYNAMICBASE /NXCOMPAT)
    
    # Treat warnings as errors only in Release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/W4 /WX)
    endif()
    
    # Disable specific warnings
    string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    
    # Enable incremental linking for faster builds
    add_link_options(/INCREMENTAL:NO)
endif()

# GCC/Clang Configuration
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -fPIC
    )
    
    # Position-independent code
    add_compile_options(-fPIE)
    add_link_options(-pie)
endif()

# ============================================================================
# Find Dependencies
# ============================================================================
# Qt6 Components
set(QT_COMPONENTS Core Gui Widgets Network Multimedia)

find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})

# Lua 5.4 (preferred) or fallback to any available version
find_package(Lua QUIET)
if(NOT Lua_FOUND)
    find_package(Lua 5.3 REQUIRED)
endif()

# ============================================================================
# Subdirectories
# ============================================================================
add_subdirectory(cmake)
add_subdirectory(bummergram)

# ============================================================================
# Installation Rules
# ============================================================================
install(TARGETS BummerGram RUNTIME DESTINATION bin)
install(DIRECTORY plugins/ DESTINATION plugins)
install(DIRECTORY bummergram/resources/ DESTINATION resources)

# Print configuration summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "  BummerGram Build Configuration")
message(STATUS "========================================")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Lua Version: ${LUA_VERSION}")
message(STATUS "  Qt6 Version: ${Qt6_VERSION}")
message(STATUS "========================================")
message(STATUS "")
