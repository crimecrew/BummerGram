# ============================================================================
# FindLua.cmake - Lua 5.4/5.3/5.2/5.1 Discovery Module
# ============================================================================
# This module finds the Lua headers and library.
#
# OUTPUT Variables:
#   LUA_FOUND        - True if Lua was found
#   LUA_INCLUDE_DIR  - Include directories for Lua headers
#   LUA_LIBRARY      - Lua library path
#   LUA_VERSION      - Detected Lua version string
#   LUA_VERSION_MAJOR - Major version number
#   LUA_VERSION_MINOR - Minor version number
#   LUA_VERSION_PATCH - Patch version number
#
# Cache Variables:
#   LUA_INCLUDE_DIR  - Lua header directory
#   LUA_LIBRARY       - Lua library file

include(FindPackageHandleStandardArgs)

# Search paths for Lua
set(_LUA_SEARCH_PATHS
    $ENV{LUA_DIR}
    $ENV{LUA_ROOT}
    $ENV{LUA_PATH}
    ${CMAKE_PREFIX_PATH}
    /usr/local
    /usr
    /opt
    /opt/local
    /sw
)

# Try to find lua.hpp first (LuaJIT and Lua 5.2+)
find_path(LUA_INCLUDE_DIR lua.hpp
    HINTS ${_LUA_SEARCH_PATHS}
    PATHS
        /usr/local/include
        /usr/include
        /opt/local/include
        /opt/include
        /sw/include
    PATH_SUFFIXES
        lua54 lua53 lua52 lua51 lua5.4 lua5.3 lua5.2 lua5.1 lua
        include include/lua
)

# Fallback to lua.h (Lua 5.1)
if(NOT LUA_INCLUDE_DIR)
    find_path(LUA_INCLUDE_DIR lua.h
        HINTS ${_LUA_SEARCH_PATHS}
        PATHS
            /usr/local/include
            /usr/include
            /opt/local/include
        PATH_SUFFIXES
            lua54 lua53 lua52 lua51 lua5.4 lua5.3 lua5.2 lua5.1 lua
            include include/lua
    )
endif()

# Find Lua library
set(_LUA_LIBRARY_NAMES lua54 lua5.4 lua53 lua5.3 lua52 lua5.2 lua51 lua5.1 lua)

find_library(LUA_LIBRARY
    NAMES ${_LUA_LIBRARY_NAMES}
    HINTS ${_LUA_SEARCH_PATHS}
    PATHS
        /usr/local/lib
        /usr/lib
        /lib
        /opt/local/lib
        /opt/lib
        /sw/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib/i386-linux-gnu
    PATH_SUFFIXES lua54 lua53 lua52 lua51 lua5.4 lua5.3 lua5.2 lua5.1 lua
)

# Extract Lua version from headers
if(LUA_INCLUDE_DIR)
    # Try lua.hpp (LuaJIT, Lua 5.2+)
    if(EXISTS "${LUA_INCLUDE_DIR}/lua.hpp")
        file(READ "${LUA_INCLUDE_DIR}/lua.hpp" _lua_header_content)
        if(_lua_header_content MATCHES "LUA_VERSION_NUM[ ]+([0-9]+)")
            set(LUA_VERSION_NUM ${CMAKE_MATCH_1})
        endif()
    endif()
    
    # Fallback to lua.h
    if(NOT LUA_VERSION_NUM AND EXISTS "${LUA_INCLUDE_DIR}/lua.h")
        file(READ "${LUA_INCLUDE_DIR}/lua.h" _lua_header_content)
        if(_lua_header_content MATCHES "LUA_VERSION_NUM[ ]+([0-9]+)")
            set(LUA_VERSION_NUM ${CMAKE_MATCH_1})
        endif()
    endif()
    
    # Parse version
    if(LUA_VERSION_NUM)
        math(EXPR LUA_VERSION_MAJOR "${LUA_VERSION_NUM} / 100")
        math(EXPR LUA_VERSION_MINOR "(${LUA_VERSION_NUM} % 100) / 10")
        math(EXPR LUA_VERSION_PATCH "${LUA_VERSION_NUM} % 10")
        set(LUA_VERSION "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_PATCH}")
    endif()
endif()

# Handle REQUIRED and QUIET arguments
find_package_handle_standard_args(Lua
    REQUIRED_VARS
        LUA_LIBRARY
        LUA_INCLUDE_DIR
    VERSION_VAR
        LUA_VERSION
)

# Provide backwards compatibility
if(LUA_FOUND)
    set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
    set(LUA_LIBRARIES ${LUA_LIBRARY})
    
    message(STATUS "Found Lua ${LUA_VERSION} (${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_PATCH})")
    message(STATUS "  Include: ${LUA_INCLUDE_DIR}")
    message(STATUS "  Library: ${LUA_LIBRARY}")
endif()

# Mark as advanced
mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARY)
