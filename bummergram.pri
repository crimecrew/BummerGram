# BummerGram.pri - Main include file for BummerGram modules
# Include this file to add all BummerGram sources to your project

# Base paths
BMMERGRAM_ROOT = $$PWD
BMMERGRAM_SOURCE_DIR = $$BMMERGRAM_ROOT/bummergram
BMMERGRAM_INCLUDE_DIR = $$BMMERGRAM_ROOT/bummergram/include

# Core modules
include($$BMMERGRAM_ROOT/bummergram/core/core.pri)

# UI modules
include($$BMMERGRAM_ROOT/bummergram/ui/ui.pri)

# Audio module
include($$BMMERGRAM_ROOT/bummergram/audio/audio.pri)

# Plugin modules
include($$BMMERGRAM_ROOT/bummergram/plugins/plugins.pri)

# Resources
RESOURCES += \
    $$BMMERGRAM_SOURCE_DIR/resources/bummergram.qrc

# Additional include paths
INCLUDEPATH += \
    $$BMMERGRAM_INCLUDE_DIR \
    $$BMMERGRAM_SOURCE_DIR/core \
    $$BMMERGRAM_SOURCE_DIR/ui/splash \
    $$BMMERGRAM_SOURCE_DIR/ui/settings \
    $$BMMERGRAM_SOURCE_DIR/ui/widgets \
    $$BMMERGRAM_SOURCE_DIR/audio \
    $$BMMERGRAM_SOURCE_DIR/plugins \
    $$BMMERGRAM_SOURCE_DIR/plugins/sandbox

# Compiler flags
DEFINES += \
    BUMMERGRAM_VERSION=\\\"1.0.0\\\" \
    BUMMERGRAM_APP_NAME=\\\"BummerGram\\\"

# Windows-specific
win32 {
    DEFINES += \
        _CRT_SECURE_NO_WARNINGS \
        WIN32_LEAN_AND_MEAN
    
    LIBS += \
        -lwinmm
}
