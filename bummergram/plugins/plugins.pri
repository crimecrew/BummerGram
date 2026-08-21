# Plugin modules PRI file

SOURCES += \
    $$PWD/plugin_manager.cpp \
    $$PWD/lua_state.cpp \
    $$PWD/lua_safe_wrappers.cpp \
    $$PWD/sandbox/lua_sandbox.cpp

HEADERS += \
    $$PWD/plugin_manager.h \
    $$PWD/lua_state.h \
    $$PWD/lua_safe_wrappers.h \
    $$PWD/sandbox/lua_sandbox.h

# Link Lua library
win32 {
    LIBS += -llua
}

unix:!macx {
    LIBS += -llua
}

macx {
    LIBS += -llua
}
