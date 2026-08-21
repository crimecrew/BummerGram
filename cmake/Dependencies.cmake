# ============================================================================
# Dependencies.cmake - External library configuration
# ============================================================================

# Qt6 Configuration
set(QT_NO_CREATE_TARGETS OFF)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Multimedia
)

# Qt6 Auto-generate moc files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 Windows-specific settings
if(WIN32)
    # Enable ANGLE for OpenGL ES emulation
    set(OPENGL_ES 3 OFF CACHE BOOL "" FORCE)
    
    # Use static Qt libraries for easier distribution
    set(QT_STATIC ON CACHE BOOL "" FORCE)
endif()

# Print found components
message(STATUS "Qt6 Components:")
foreach(comp ${QT_COMPONENTS})
    message(STATUS "  - ${comp}")
endforeach()
