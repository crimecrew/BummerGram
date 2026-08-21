# Audio modules PRI file

SOURCES += \
    $$PWD/startup_sound.cpp \
    $$PWD/audio_player.cpp

HEADERS += \
    $$PWD/startup_sound.h \
    $$PWD/audio_player.h

# Audio requires multimedia on Windows
win32 {
    LIBS += -lwinmm
}
