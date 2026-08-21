# ============================================================================
# CompilerFlags.cmake - Cross-platform compiler flag configuration
# ============================================================================

# Check for threading support
find_package(Threads REQUIRED)

# MSVC Flags
if(MSVC)
    # Warning level
    add_compile_options(/W4)
    
    # Disable warnings that are commonly triggered in Qt code
    add_compile_options(
        /wd4127   # conditional expression is constant
        /wd4244   # conversion, possible loss of data
        /wd4267   # conversion from size_t
        /wd4474   # too many actual parameters
        /wd4714   # function marked as inline
        /wd4251   # DLL interface needed
    )
    
    # Security flags
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
        UNICODE
        _UNICODE
    )
    
    # Optimization flags for Release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /Oi /Ot)
    endif()
    
    # Linker flags
    add_link_options(/DYNAMICBASE)
    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Warning flags
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )
    
    # Security flags
    add_compile_options(
        -fstack-protector-strong
        -D_FORTIFY_SOURCE=2
    )
    
    # Position independent code
    add_compile_options(-fPIC -fPIE)
    add_link_options(-pie)
    
    # Optimization
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
endif()

# Common defines for all compilers
add_compile_definitions(
    QT_DEPRECATED_WARNINGS
    QT_NO_DEBUG
    NOMINMAX
)
